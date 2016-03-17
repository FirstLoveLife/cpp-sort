/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Morwenn
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
#ifndef CPPSORT_SORTER_TRAITS_H_
#define CPPSORT_SORTER_TRAITS_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <cstddef>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>
#include <cpp-sort/fwd.h>
#include <cpp-sort/utility/as_function.h>
#include <cpp-sort/utility/detection.h>
#include <cpp-sort/utility/functional.h>
#include <cpp-sort/utility/is_callable.h>
#include "detail/raw_checkers.h"

namespace cppsort
{
    ////////////////////////////////////////////////////////////
    // Whether a type is a projection

    namespace detail
    {
        template<
            typename Projection, typename Iterable, typename Compare,
            typename ProjFunc = decltype(utility::as_function(std::declval<Projection&>()))
        >
        using is_projection_t = std::result_of_t<Compare(
            std::result_of_t<ProjFunc(decltype(*std::begin(std::declval<Iterable&>())))>,
            std::result_of_t<ProjFunc(decltype(*std::end(std::declval<Iterable&>())))>
        )>;

        template<
            typename Projection, typename Iterator, typename Compare,
            typename ProjFunc = decltype(utility::as_function(std::declval<Projection&>()))
        >
        using is_projection_iterator_t = std::result_of_t<Compare(
            std::result_of_t<ProjFunc(decltype(*std::declval<Iterator&>()))>,
            std::result_of_t<ProjFunc(decltype(*std::declval<Iterator&>()))>
        )>;
    }

    template<
        typename Projection,
        typename Iterable,
        typename Compare = std::less<>
    >
    constexpr bool is_projection
        = utility::is_detected_v<detail::is_projection_t, Projection, Iterable, Compare>;

    template<
        typename Projection,
        typename Iterator,
        typename Compare = std::less<>
    >
    constexpr bool is_projection_iterator
        = utility::is_detected_v<detail::is_projection_iterator_t, Projection, Iterator, Compare>;

    ////////////////////////////////////////////////////////////
    // Whether a type is a three-way comparator

    namespace detail
    {
        template<
            typename Compare, typename Iterable,
            typename Projection, typename Ret,
            typename ProjFunc = decltype(utility::as_function(std::declval<Projection&>()))
        >
        constexpr bool is_three_way_comp_impl =
            utility::is_callable<Compare(
                std::result_of_t<ProjFunc(decltype(*std::begin(std::declval<Iterable&>())))>,
                std::result_of_t<ProjFunc(decltype(*std::end(std::declval<Iterable&>())))>
            ), Ret>::value;

        template<
            typename Compare, typename Iterator,
            typename Projection, typename Ret,
            typename ProjFunc = decltype(utility::as_function(std::declval<Projection&>()))
        >
        constexpr bool is_three_way_comp_it_impl =
            utility::is_callable<Compare(
                std::result_of_t<ProjFunc(decltype(*std::declval<Iterator&>()))>,
                std::result_of_t<ProjFunc(decltype(*std::declval<Iterator&>()))>
            ), Ret>::value;
    }

    template<
        typename Compare, typename Iterable,
        typename Projection = utility::identity
    >
    constexpr bool is_three_way_comparison =
        detail::is_three_way_comp_impl<Compare, Iterable, Projection, utility::partial_ordering> ||
        detail::is_three_way_comp_impl<Compare, Iterable, Projection, utility::weak_ordering> ||
        detail::is_three_way_comp_impl<Compare, Iterable, Projection, utility::total_ordering>;

    template<
        typename Compare, typename Iterator,
        typename Projection = utility::identity
    >
    constexpr bool is_three_way_comparison_iterator =
        detail::is_three_way_comp_it_impl<Compare, Iterator, Projection, utility::partial_ordering> ||
        detail::is_three_way_comp_it_impl<Compare, Iterator, Projection, utility::weak_ordering> ||
        detail::is_three_way_comp_it_impl<Compare, Iterator, Projection, utility::total_ordering>;

    ////////////////////////////////////////////////////////////
    // Sorter type categories

    namespace detail
    {
        // Group two iterators as a range, used to discriminate
        // sorters from comparison functions
        template<typename Iterator>
        class range
        {
            public:

                range(Iterator begin, Iterator end):
                    _begin(begin),
                    _end(end)
                {}

                Iterator begin() { return _begin; }
                Iterator end() { return _end; }
                Iterator begin() const { return _begin; }
                Iterator end() const { return _end; }

            private:

                Iterator _begin, _end;
        };

        template<typename Sorter, typename Iterable>
        using has_sort_t = std::result_of_t<Sorter(Iterable&)>;

        template<typename Sorter, typename Iterable, typename Compare>
        using has_comparison_sort_t = std::result_of_t<Sorter(Iterable&, Compare)>;

        template<typename Sorter, typename Iterable, typename Projection>
        using has_projection_sort_t = std::result_of_t<Sorter(Iterable&, Projection)>;

        template<
            typename Sorter, typename Iterable,
            typename Compare, typename Projection
        >
        using has_comparison_projection_sort_t
            = std::result_of_t<Sorter(Iterable&, Compare, Projection)>;

        template<typename Sorter, typename Iterator>
        using has_sort_iterator_t = std::result_of_t<Sorter(Iterator, Iterator)>;

        template<typename Sorter, typename Iterator, typename Compare>
        using has_comparison_sort_iterator_t = std::result_of_t<Sorter(Iterator, Iterator, Compare)>;

        template<typename Sorter, typename Iterator, typename Projection>
        using has_projection_sort_iterator_t = std::result_of_t<Sorter(Iterator, Iterator, Projection)>;

        template<
            typename Sorter, typename Iterator,
            typename Compare, typename Projection
        >
        using has_comparison_projection_sort_iterator_t
            = std::result_of_t<Sorter(Iterator, Iterator, Compare, Projection)>;

        template<typename Sorter, typename Iterable>
        constexpr bool has_sort
            = utility::is_detected_v<has_sort_t, Sorter, Iterable>;

        template<typename Sorter, typename Iterable, typename Compare>
        constexpr bool has_comparison_sort
            = utility::is_detected_v<has_comparison_sort_t, Sorter, Iterable, Compare> &&
              is_projection<utility::identity, Iterable, Compare>;

        template<typename Sorter, typename Iterable, typename Projection>
        constexpr bool has_projection_sort
            = utility::is_detected_v<has_projection_sort_t, Sorter, Iterable, Projection> &&
              is_projection<Projection, Iterable>;

        template<
            typename Sorter, typename Iterable,
            typename Compare, typename Projection
        >
        constexpr bool has_comparison_projection_sort
            = utility::is_detected_v<
                has_comparison_projection_sort_t,
                Sorter, Iterable, Compare, Projection
            > &&
            is_projection<Projection, Iterable, Compare>;

        template<typename Sorter, typename Iterator>
        constexpr bool has_sort_iterator
            = utility::is_detected_v<has_sort_iterator_t, Sorter, Iterator>;

        template<typename Sorter, typename Iterator, typename Compare>
        constexpr bool has_comparison_sort_iterator
            = utility::is_detected_v<has_comparison_sort_iterator_t, Sorter, Iterator, Compare> &&
              is_projection_iterator<utility::identity, Iterator, Compare>;

        template<typename Sorter, typename Iterator, typename Projection>
        constexpr bool has_projection_sort_iterator
            = utility::is_detected_v<has_projection_sort_iterator_t, Sorter, Iterator, Projection> &&
              is_projection_iterator<Projection, Iterator>;

        template<
            typename Sorter, typename Iterator,
            typename Compare, typename Projection
        >
        constexpr bool has_comparison_projection_sort_iterator
            = utility::is_detected_v<
                has_comparison_projection_sort_iterator_t,
                Sorter, Iterator, Compare, Projection
            > &&
            is_projection_iterator<Projection, Iterator, Compare>;
    }

    template<typename Sorter, typename Iterable>
    constexpr bool is_sorter
        = detail::has_sort<Sorter, Iterable>;

    template<typename Sorter, typename Iterable, typename Compare>
    constexpr bool is_comparison_sorter
        = detail::has_comparison_sort<Sorter, Iterable, Compare>;

    template<typename Sorter, typename Iterable, typename Projection>
    constexpr bool is_projection_sorter
        = detail::has_projection_sort<Sorter, Iterable, Projection>;

    template<typename Sorter, typename Iterable,
             typename Compare, typename Projection>
    constexpr bool is_comparison_projection_sorter
        = detail::has_comparison_projection_sort<Sorter, Iterable, Compare, Projection>;

    template<typename Sorter, typename Iterator>
    constexpr bool is_sorter_iterator
        = is_sorter<Sorter, detail::range<Iterator>> &&
          detail::has_sort_iterator<Sorter, Iterator>;

    template<typename Sorter, typename Iterator, typename Compare>
    constexpr bool is_comparison_sorter_iterator
        = is_comparison_sorter<Sorter, detail::range<Iterator>, Compare> &&
          detail::has_comparison_sort_iterator<Sorter, Iterator, Compare>;

    template<typename Sorter, typename Iterator, typename Projection>
    constexpr bool is_projection_sorter_iterator
        = is_projection_sorter<Sorter, detail::range<Iterator>, Projection> &&
          detail::has_projection_sort_iterator<Sorter, Iterator, Projection>;

    template<typename Sorter, typename Iterator,
             typename Compare, typename Projection>
    constexpr bool is_comparison_projection_sorter_iterator
        = is_comparison_projection_sorter<Sorter, detail::range<Iterator>, Compare, Projection> &&
          detail::has_comparison_projection_sort_iterator<Sorter, Iterator, Compare, Projection>;

    ////////////////////////////////////////////////////////////
    // Sorter traits

    // This trait class is a bit different than usual traits
    // classes: the goal is to decrease the coupling between
    // the different traits and to make programs using one of
    // the traits valid even if the other traits don't exist

    template<typename Sorter>
    struct sorter_traits:
        detail::raw_check_iterator_category<Sorter>,
        detail::raw_check_is_stable<Sorter>
    {};

    template<typename Sorter>
    using iterator_category = typename sorter_traits<Sorter>::iterator_category;

    template<typename Sorter>
    using is_stable = typename sorter_traits<Sorter>::is_stable;

    ////////////////////////////////////////////////////////////
    // Fixed-size sorter traits

    template<template<std::size_t> class FixedSizeSorter>
    struct fixed_sorter_traits
    {
        // Empty for SFINAE-friendliness
    };

    ////////////////////////////////////////////////////////////
    // Sorter traits modifiers

    template<typename Sorter, typename Category>
    struct rebind_iterator_category:
        Sorter
    {
        static_assert(
            std::is_base_of<
                cppsort::iterator_category<Sorter>,
                Category
            >::value,
            "the new iterator category should be more specific"
        );

        // New category
        using iterator_category = Category;
    };
}

#endif // CPPSORT_SORTER_TRAITS_H_
