/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Morwenn
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef CPPSORT_DETAIL_LOW_MOVES_SORT_N_H_
#define CPPSORT_DETAIL_LOW_MOVES_SORT_N_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <algorithm>
#include <cstddef>
#include <utility>
#include <cpp-sort/sorter_facade.h>

namespace cppsort
{
namespace detail
{
    template<std::size_t N>
    struct low_moves_sorter_n:
        sorter_facade<low_moves_sorter_n<N>>
    {
        using sorter_facade<low_moves_sorter_n<N>>::operator();

        template<typename RandomAccessIterator, typename Compare>
        auto operator()(RandomAccessIterator first, RandomAccessIterator last,
                        Compare compare) const
            -> void
        {
            RandomAccessIterator min = std::min_element(first, last, compare);
            if (first != min)
            {
                std::iter_swap(first, min);
            }
            low_moves_sorter_n<N-1>{}(++first, last, compare);
        }
    };

    template<
        std::size_t N,
        typename... Args
    >
    auto low_moves_sort_n(Args&&... args)
        -> decltype(auto)
    {
        return low_moves_sorter_n<N>{}(std::forward<Args>(args)...);
    }
}}

// Specializations of low_moves_sorter_n for some values of N
#include "sort0.h"
#include "sort1.h"
#include "sort2.h"
#include "sort3.h"

#endif // CPPSORT_DETAIL_LOW_MOVES_SORT_N_H_