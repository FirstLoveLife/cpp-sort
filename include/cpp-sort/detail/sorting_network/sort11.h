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
#ifndef CPPSORT_DETAIL_SORTING_NETWORK_SORT11_H_
#define CPPSORT_DETAIL_SORTING_NETWORK_SORT11_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <functional>
#include "../swap_if.h"

namespace cppsort
{
namespace detail
{
    template<>
    struct sorting_network_sorter_impl<11u>
    {
        template<
            typename RandomAccessIterator,
            typename Compare = std::less<>
        >
        auto operator()(RandomAccessIterator first, RandomAccessIterator, Compare compare={}) const
            -> void
        {
            swap_if(first[0u], first[1u], compare);
            swap_if(first[2u], first[3u], compare);
            swap_if(first[4u], first[5u], compare);
            swap_if(first[6u], first[7u], compare);
            swap_if(first[8u], first[9u], compare);
            swap_if(first[1u], first[3u], compare);
            swap_if(first[5u], first[7u], compare);
            swap_if(first[0u], first[2u], compare);
            swap_if(first[4u], first[6u], compare);
            swap_if(first[8u], first[10u], compare);
            swap_if(first[1u], first[2u], compare);
            swap_if(first[5u], first[6u], compare);
            swap_if(first[9u], first[10u], compare);
            swap_if(first[1u], first[5u], compare);
            swap_if(first[6u], first[10u], compare);
            swap_if(first[5u], first[9u], compare);
            swap_if(first[2u], first[6u], compare);
            swap_if(first[1u], first[5u], compare);
            swap_if(first[6u], first[10u], compare);
            swap_if(first[0u], first[4u], compare);
            swap_if(first[3u], first[7u], compare);
            swap_if(first[4u], first[8u], compare);
            swap_if(first[0u], first[4u], compare);
            swap_if(first[1u], first[4u], compare);
            swap_if(first[7u], first[10u], compare);
            swap_if(first[3u], first[8u], compare);
            swap_if(first[2u], first[3u], compare);
            swap_if(first[8u], first[9u], compare);
            swap_if(first[2u], first[4u], compare);
            swap_if(first[7u], first[9u], compare);
            swap_if(first[3u], first[5u], compare);
            swap_if(first[6u], first[8u], compare);
            swap_if(first[3u], first[4u], compare);
            swap_if(first[5u], first[6u], compare);
            swap_if(first[7u], first[8u], compare);
        }
    };
}}

#endif // CPPSORT_DETAIL_SORTING_NETWORK_SORT11_H_
