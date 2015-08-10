/*
 * Copyright (C) 2015 Morwenn
 *
 * array_sort is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * array_sort is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program. If not,
 * see <http://www.gnu.org/licenses/>.
 */
#ifndef CPPSORT_SORT_H_
#define CPPSORT_SORT_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <functional>
#include <type_traits>
#include <cpp-sort/sorters/default_sorter.h>
#include <cpp-sort/utility/is_sorter_for.h>

namespace cppsort
{
    template<
        typename RandomAccessIterable,
        typename Compare = std::less<>,
        typename = std::enable_if_t<not utility::is_sorter_for<Compare, RandomAccessIterable>>
    >
    auto sort(RandomAccessIterable& iterable, Compare compare={})
        -> void
    {
        sort(iterable, default_sorter{}, compare);
    }

    template<
        typename RandomAccessIterable,
        typename Sorter,
        typename Compare = std::less<>
    >
    auto sort(RandomAccessIterable& iterable, const Sorter& sorter, Compare compare={})
        -> void
    {
        sorter(iterable, compare);
    }
}

#endif // CPPSORT_SORT_H_
