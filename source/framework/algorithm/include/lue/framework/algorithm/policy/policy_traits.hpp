#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <limits>
#include <tuple>


namespace lue::policy {
    namespace detail {

        template<
            typename T>
        class TypeTraits
        {

            public:

                static constexpr T no_data_value{
                    []()
                    {
                        if constexpr (std::is_floating_point_v<T>)
                        {
                            return std::numeric_limits<T>::quiet_NaN();
                        }
                        else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>)
                        {
                            return std::numeric_limits<T>::min();
                        }
                        else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>)
                        {
                            return std::numeric_limits<T>::max();
                        }
                    }()};

        };


        template<
            typename Policies>
        class PoliciesTraits
        {

            public:

                using DomainPolicy = typename Policies::DomainPolicy;

                using OutputsPolicies = typename Policies::OutputsPolicies;

                using InputsPolicies = typename Policies::InputsPolicies;

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
        typename detail::TypeTraits<
                lue::detail::remove_cvref_t<T>
            >::Element;


    template<
        typename Policies,
        std::size_t idx>
    using PoliciesT =
        typename detail::TypeTraits<
                lue::detail::remove_cvref_t<Policies>
            >::template Policies<idx>;


    template<
        // All policies related to an operation
        typename Policies,
        // Idx of policies related to a specific output
        std::size_t idx,
        // Element for which to return an output policies type
        typename Element=ElementT<PoliciesT<typename Policies::OutputsPolicies, idx>>>
    using OutputPoliciesT =
        typename detail::TypeTraits<
                // Policies related to a specific output
                PoliciesT<typename Policies::OutputsPolicies, idx>
            >::template Policies<Element>;


    template<
        // All policies related to an operation
        typename Policies,
        // Idx of policies related to a specific input
        std::size_t idx,
        // Element for which to return an input policies type
        typename Element=ElementT<PoliciesT<typename Policies::InputsPolicies, idx>>>
    using InputPoliciesT =
        typename detail::TypeTraits<
                // Policies related to a specific input
                PoliciesT<typename Policies::InputsPolicies, idx>
            >::template Policies<Element>;


    template<
        typename OutputPolicies>
    using OutputNoDataPolicy2T =
        typename detail::TypeTraits<
                lue::detail::remove_cvref_t<OutputPolicies>
            >::OutputNoDataPolicy;


    template<
        typename OutputNoDataPolicy,
        typename Element>
    using OutputNoDataPolicyT =
        typename detail::TypeTraits<
                lue::detail::remove_cvref_t<OutputNoDataPolicy>
            >::template Policy<Element>;


    template<
        // All policies related to an operation
        typename Policies,
        // Idx of policies related to a specific output
        std::size_t idx,
        // Element for which to return an output no-data policies type
        typename Element=ElementT<PoliciesT<typename Policies::OutputsPolicies, idx>>>
    using OutputNoDataPolicy3T =
        OutputNoDataPolicy2T<
            typename detail::TypeTraits<
                    // Policies related to a specific output
                    PoliciesT<typename Policies::OutputsPolicies, idx>
                >::template Policies<Element>>;


    // template<
    //     typename RangePolicy,
    //     typename Element>
    // using RangePolicyT =
    //     typename detail::TypeTraits<
    //             lue::detail::remove_cvref_t<RangePolicy>
    //         >::template Policy<Element>;


    template<
        typename InputPolicies>
    using InputNoDataPolicy2T =
        typename detail::TypeTraits<
                lue::detail::remove_cvref_t<InputPolicies>
            >::InputNoDataPolicy;


    template<
        typename InputNoDataPolicy,
        typename Element>
    using InputNoDataPolicyT =
        typename detail::TypeTraits<
                lue::detail::remove_cvref_t<InputNoDataPolicy>
            >::template Policy<Element>;


    template<
        typename HaloPolicy,
        typename Element>
    using HaloPolicyT =
        typename detail::TypeTraits<
                lue::detail::remove_cvref_t<HaloPolicy>
            >::template Policy<Element>;


    template<
        typename Element>
    Element no_data_value = detail::TypeTraits<lue::detail::remove_cvref_t<Element>>::no_data_value;

}  //  namespace lue::policy
