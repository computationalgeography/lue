#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <limits>
#include <tuple>


namespace lue::policy {
    namespace detail {

        template<
            typename T,
            typename Enable=void>
        class TypeTraits
        {
        };


        template<
            typename T>
        class TypeTraits<
            T,
            typename std::enable_if_t<std::is_floating_point_v<T>>>
        {

            public:

                static constexpr T no_data_value = std::numeric_limits<T>::quiet_NaN();

        };


        template<
            typename T>
        class TypeTraits<
            T,
            typename std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>>>
        {

            public:

                static T const no_data_value = std::numeric_limits<T>::min();

        };


        template<
            typename T>
        class TypeTraits<
            T,
            typename std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>>>
        {

            public:

                static T const no_data_value = std::numeric_limits<T>::max();

        };


        template<
            typename Policies>
        class PoliciesTraits
        {
        };

    }    // namespace detail


    template<
        typename Policies>
    using DomainPolicyT = typename detail::PoliciesTraits<Policies>::DomainPolicy;


    template<
        typename Policies>
    using OutputsPoliciesT = typename detail::PoliciesTraits<Policies>::OutputsPolicies;


    template<
        typename Policies>
    using InputsPoliciesT = typename detail::PoliciesTraits<Policies>::InputsPolicies;


    template<
        typename T>
    using ElementT =
        lue::detail::remove_cvref_t<
                typename detail::TypeTraits<T>::Element
            >;


    template<
        typename Policies,
        std::size_t idx>
    using PoliciesT =
        lue::detail::remove_cvref_t<
                typename detail::TypeTraits<Policies>::template Policies<idx>
            >;


    template<
        // All policies related to an operation
        typename Policies,
        // Idx of policies related to a specific output
        std::size_t idx,
        // Element for which to return an output policies type
        typename Element=ElementT<PoliciesT<typename Policies::OutputsPolicies, idx>>>
    using OutputPoliciesT =
        lue::detail::remove_cvref_t<
                typename detail::TypeTraits<
                        // Policies related to a specific output
                        PoliciesT<typename Policies::OutputsPolicies, idx>
                    >::template Policies<Element>
            >;


    template<
        // All policies related to an operation
        typename Policies,
        // Idx of policies related to a specific output
        std::size_t idx,
        // Element for which to return an output policies type
        typename Element=ElementT<PoliciesT<typename Policies::InputsPolicies, idx>>>
    using InputPoliciesT =
        lue::detail::remove_cvref_t<
                typename detail::TypeTraits<
                        // Policies related to a specific output
                        PoliciesT<typename Policies::InputsPolicies, idx>
                    >::template Policies<Element>
            >;


    template<
        typename OutputNoDataPolicy,
        typename Element>
    using OutputNoDataPolicyT =
        lue::detail::remove_cvref_t<
                typename detail::TypeTraits<OutputNoDataPolicy>::template Policy<Element>
            >;


    template<
        typename InputNoDataPolicy,
        typename Element>
    using InputNoDataPolicyT =
        lue::detail::remove_cvref_t<
                typename detail::TypeTraits<InputNoDataPolicy>::template Policy<Element>
            >;


    template<
        typename HaloPolicy,
        typename Element>
    using HaloPolicyT =
        lue::detail::remove_cvref_t<
                typename detail::TypeTraits<HaloPolicy>::template Policy<Element>
            >;


    template<
        typename Element>
    Element no_data_value = detail::TypeTraits<Element>::no_data_value;

}  //  namespace lue::policy
