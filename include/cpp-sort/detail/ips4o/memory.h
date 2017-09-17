/*
 * ips4o/memory.hpp
 *
 * In-place Parallel Super Scalar Samplesort (IPS⁴o)
 *
 ******************************************************************************
 * BSD 2-Clause License
 *
 * Copyright © 2017, Michael Axtmann <michael.axtmann@kit.edu>
 * Copyright © 2017, Daniel Ferizovic <daniel.ferizovic@student.kit.edu>
 * Copyright © 2017, Sascha Witt <sascha.witt@kit.edu>
 * All rights reserved.
 *
 * Modified in 2017 by Morwenn for inclusion into cpp-sort.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef CPPSORT_DETAIL_IPS4O_MEMORY_H_
#define CPPSORT_DETAIL_IPS4O_MEMORY_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <random>
#include <utility>
#include <vector>
#include "ips4o_fwd.h"
#include "bucket_pointers.h"
#include "buffers.h"
#include "classifier.h"

namespace cppsort
{
namespace detail
{
namespace ips4o
{
namespace detail
{
    /*
     * Aligns a pointer.
     */
    template<typename T>
    static auto alignPointer(T* ptr, std::size_t alignment)
        -> T*
    {
        std::uintptr_t v = reinterpret_cast<std::uintptr_t>(ptr);
        v = (v - 1 + alignment) & ~(alignment - 1);
        return reinterpret_cast<T*>(v);
    }

    /*
     * Constructs an object at the specified alignment.
     */
    template<typename T>
    class AlignedPtr
    {
        public:

            AlignedPtr() {}

            template<typename... Args>
            explicit AlignedPtr(std::size_t alignment, Args&&... args):
                alloc_(new char[sizeof(T) + alignment]),
                value_(new (alignPointer(alloc_, alignment)) T(std::forward<Args>(args)...))
            {}

            AlignedPtr(const AlignedPtr&) = delete;
            AlignedPtr& operator=(const AlignedPtr&) = delete;

            AlignedPtr(AlignedPtr&& rhs):
                alloc_(rhs.alloc_), value_(rhs.value_)
            {
                rhs.alloc_ = nullptr;
            }

            auto operator=(AlignedPtr&& rhs)
                -> AlignedPtr&
            {
                using std::swap;
                swap(alloc_, rhs.alloc_);
                swap(value_, rhs.value_);
                return *this;
            }

            ~AlignedPtr()
            {
                if (alloc_) {
                    value_->~T();
                    delete[] alloc_;
                }
            }

            auto get()
                -> T&
            {
                return *value_;
            }

        private:

            char* alloc_ = nullptr;
            T* value_;
    };

    /*
     * Provides aligned storage without constructing an object.
     */
    template<>
    class AlignedPtr<void>
    {
        public:

            AlignedPtr() {}

            template<typename... Args>
            explicit AlignedPtr(std::size_t alignment, std::size_t size):
                alloc_(new char[size + alignment]),
                value_(alignPointer(alloc_, alignment))
            {}

            AlignedPtr(const AlignedPtr&) = delete;
            AlignedPtr& operator=(const AlignedPtr&) = delete;

            AlignedPtr(AlignedPtr&& rhs):
                alloc_(rhs.alloc_), value_(rhs.value_)
            {
                rhs.alloc_ = nullptr;
            }

            auto operator=(AlignedPtr&& rhs)
                -> AlignedPtr&
            {
                using std::swap;
                swap(alloc_, rhs.alloc_);
                swap(value_, rhs.value_);
                return *this;
            }

            ~AlignedPtr()
            {
                if (alloc_) {
                    delete[] alloc_;
                }
            }

            auto get()
                -> char*
            {
                return value_;
            }

        private:

            char* alloc_ = nullptr;
            char* value_;
    };

    /*
     * Aligned storage for use in buffers.
     */
    template<typename Cfg>
    class Sorter<Cfg>::BufferStorage:
        public AlignedPtr<void>
    {
        static constexpr auto kPerThread =
                Cfg::kBlockSizeInBytes * Cfg::kMaxBuckets * (1 + Cfg::kAllowEqualBuckets);

        public:

            BufferStorage() {}

            explicit BufferStorage(int num_threads):
                AlignedPtr<void>(Cfg::kDataAlignment, num_threads * kPerThread)
            {}

            auto forThread(int id)
                -> char*
            {
                return this->get() + id * kPerThread;
            }
    };

    /*
     * Data local to each thread.
     */
    template<typename Cfg>
    struct Sorter<Cfg>::LocalData
    {
        using diff_t = typename Cfg::difference_type;

        // Buffers
        diff_t bucket_size[Cfg::kMaxBuckets];
        Buffers buffers;
        Block swap[2];
        Block overflow;

        // Bucket information
        BucketPointers bucket_pointers[Cfg::kMaxBuckets];

        // Classifier
        Classifier classifier;

        // Information used during empty block movement
        diff_t first_block;
        diff_t first_empty_block;

        // Random bit generator for sampling
        // LCG using constants by Knuth (for 64 bit) or Numerical Recipes (for 32 bit)
        std::linear_congruential_engine<
            std::uintptr_t,
            Cfg::kIs64Bit ? 6364136223846793005u : 1664525u,
            Cfg::kIs64Bit ? 1442695040888963407u : 1013904223u,
            0u
        > random_generator;

        LocalData(typename Cfg::compare_type compare, char* buffer_storage):
            buffers(buffer_storage),
            classifier(std::move(compare))
        {
            std::random_device rdev;
            std::ptrdiff_t seed = rdev();
            if (Cfg::kIs64Bit) {
                seed = (seed << (Cfg::kIs64Bit * 32)) | rdev();
            }
            random_generator.seed(seed);
            reset();
        }

        /*
         * Resets local data after partitioning is done.
         */
        auto reset()
            -> void
        {
            classifier.reset();
            std::fill_n(bucket_size, Cfg::kMaxBuckets, 0);
        }
    };

    /*
     * A subtask in the parallel algorithm.
     * Uses indices instead of iterators to avoid unnecessary template instantiations.
     */
    struct ParallelTask
    {
        std::ptrdiff_t begin;
        std::ptrdiff_t end;
        int level;

        bool operator==(const ParallelTask& rhs) const { return begin == rhs.begin && end == rhs.end; }
        bool operator!=(const ParallelTask& rhs) const { return begin != rhs.begin || end != rhs.end; }
        bool operator<(const ParallelTask& rhs) const { return end - begin < rhs.end - rhs.begin; }
        bool operator<=(const ParallelTask& rhs) const { return end - begin <= rhs.end - rhs.begin; }
        bool operator>(const ParallelTask& rhs) const { return end - begin > rhs.end - rhs.begin; }
        bool operator>=(const ParallelTask& rhs) const { return end - begin >= rhs.end - rhs.begin; }
    };

    /*
     * Data shared between all threads.
     */
    template<typename Cfg>
    struct Sorter<Cfg>::SharedData
    {
        // Bucket information
        typename Cfg::difference_type bucket_start[Cfg::kMaxBuckets + 1];
        BucketPointers bucket_pointers[Cfg::kMaxBuckets];
        Block* overflow;
        int num_buckets;
        bool use_equal_buckets;

        // Classifier for parallel partitioning
        Classifier classifier;

        // Synchronisation support
        typename Cfg::Sync sync;

        // Local thread data
        std::vector<LocalData*> local;

        // Parallel subtask information
        typename Cfg::iterator begin_;
        std::vector<ParallelTask> big_tasks;
        std::vector<ParallelTask> small_tasks;
        std::atomic_size_t small_task_index;

        SharedData(typename Cfg::compare_type compare, typename Cfg::Sync sync,
                   std::size_t num_threads):
            classifier(std::move(compare)),
            sync(std::forward<typename Cfg::Sync>(sync)),
            local(num_threads)
        {
            reset();
        }

        /*
         * Resets shared data after partitioning is done.
         */
        auto reset()
            -> void
        {
            classifier.reset();
            std::fill_n(bucket_start, Cfg::kMaxBuckets + 1, 0);
            overflow = nullptr;
        }
    };
}}}}

#endif // CPPSORT_DETAIL_IPS4O_MEMORY_H_
