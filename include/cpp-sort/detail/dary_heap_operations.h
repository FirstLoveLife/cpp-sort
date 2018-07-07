// boost heap: d-ary heap as containter adaptor
//
// Copyright (C) 2010 Tim Blechmann
// Modified in 2016-2018 by Morwenn for inclusion into cpp-sort
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef CPPSORT_DETAIL_DARY_HEAP_OPERATIONS_H_
#define CPPSORT_DETAIL_DARY_HEAP_OPERATIONS_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>
#include <cpp-sort/utility/as_function.h>
#include <cpp-sort/utility/iter_move.h>
#include "max_element.h"

namespace cppsort
{
namespace detail
{
    template<int D, typename RandomAccessIterator>
    auto parent_it(RandomAccessIterator first, RandomAccessIterator it)
        -> RandomAccessIterator
    {
        auto index = std::distance(first, it);
        return first + (index - 1) / D;
    }

    template<int D, typename RandomAccessIterator>
    auto first_child_it(RandomAccessIterator first, RandomAccessIterator it)
        -> RandomAccessIterator
    {
        auto index = std::distance(first, it);
        return first + index * D + 1;
    }

    template<int D, typename RandomAccessIterator, typename Compare, typename Projection>
    auto top_child_it(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator it,
                      Compare compare, Projection projection)
        -> RandomAccessIterator
    {
        // invariant: it is not a leaf, so the iterator range is not empty

        auto first_child = first_child_it<D>(first, it);
        auto max_elements = std::distance(first_child, last);
        auto last_child = (max_elements > D) ? first_child + D : last;

        return detail::max_element(first_child, last_child, std::move(compare), std::move(projection));
    }

    template<int D, typename RandomAccessIterator>
    auto not_leaf(RandomAccessIterator first, RandomAccessIterator last,
                  RandomAccessIterator it)
        -> bool
    {
        auto first_child = first_child_it<D>(first, it);
        return first_child < last;
    }

    template<int D, typename RandomAccessIterator, typename Compare, typename Projection>
    void siftdown(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator it,
                  Compare compare, Projection projection)
    {
        using utility::iter_swap;

        auto&& comp = utility::as_function(compare);
        auto&& proj = utility::as_function(projection);

        while (not_leaf<D>(first, last, it)) {
            auto max_child = top_child_it<D>(first, last, it, compare, projection);
            if (not comp(proj(*max_child), proj(*it))) {
                iter_swap(max_child, it);
                it = max_child;
            } else {
                return;
            }
        }
    }

    template<int D, typename RandomAccessIterator, typename Compare, typename Projection>
    void siftup(RandomAccessIterator first, RandomAccessIterator it,
                Compare compare, Projection projection)
    {
        using utility::iter_swap;

        auto&& comp = utility::as_function(compare);
        auto&& proj = utility::as_function(projection);

        while (it != first) {
            auto parent = parent_it<D>(first, it);

            if (comp(proj(*parent), proj(*it))) {
                iter_swap(parent, it);
                it = parent;
            } else {
                return;
            }
        }
    }

    template<int D, typename RandomAccessIterator, typename Compare, typename Projection>
    auto push_d_ary_heap(RandomAccessIterator first, RandomAccessIterator last,
                         Compare compare, Projection projection)
        -> void
    {
        siftup<D>(first, std::prev(last), std::move(compare), std::move(projection));
    }

    template<int D, typename RandomAccessIterator, typename Compare, typename Projection>
    auto pop_d_ary_heap(RandomAccessIterator first, RandomAccessIterator last,
                        Compare compare, Projection projection)
        -> void
    {
        using utility::iter_swap;
        iter_swap(first, --last);
        if (first == last) return;

        siftdown<D>(first, last, first, std::move(compare), std::move(projection));
    }

    template<int D, typename RandomAccessIterator, typename Compare, typename Projection>
    auto make_d_ary_heap(RandomAccessIterator first, RandomAccessIterator last,
                         Compare compare, Projection projection)
        -> void
    {
        if (first == last) return;

        for (auto it = std::next(first) ; it != last ; ++it) {
            push_d_ary_heap<D>(first, it, compare, projection);
        }
        // Take the last element into consideration
        push_d_ary_heap<D>(first, last, std::move(compare), std::move(projection));
    }

    template<int D, typename RandomAccessIterator, typename Compare, typename Projection>
    auto sort_d_ary_heap(RandomAccessIterator first, RandomAccessIterator last,
                         Compare compare, Projection projection)
        -> void
    {
        for (auto it = last ; it != first ; --it) {
            pop_d_ary_heap<D>(first, it, compare, projection);
        }
    }
}}

#endif // CPPSORT_DETAIL_DARY_HEAP_OPERATIONS_H_
