#pragma once
#include "lue/framework/algorithm/policy/detect_no_data_by_nan.hpp"
#include "lue/framework/algorithm/policy/detect_no_data_by_value.hpp"
#include "lue/framework/algorithm/policy/mark_no_data_by_nan.hpp"
#include "lue/framework/algorithm/policy/mark_no_data_by_value.hpp"
#include "lue/framework/core/type_traits.hpp"
#include <tuple>


namespace lue::policy {
    namespace detail {

        template<typename T>
        class TypeTraits
        {

            public:

                inline static constexpr auto no_data_value() -> T
                {
                    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);

                    if constexpr (std::is_floating_point_v<T>)
                    {
                        return DetectNoDataByNaN<T>::no_data_value;
                    }
                    else if constexpr (std::is_integral_v<T>)
                    {
                        return DetectNoDataByValue<T>::no_data_value;
                    }
                }

                // This stopped working in Github workflow + Windows + MSVC
                // static constexpr T no_data_value =
                //     []()
                //     {
                //         static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);

                //         if constexpr (std::is_floating_point_v<T>)
                //         {
                //             return DetectNoDataByNaN<T>::no_data_value;
                //         }
                //         else if constexpr (std::is_integral_v<T>)
                //         {
                //             return DetectNoDataByValue<T>::no_data_value;
                //         }
                //     }();
        };


        template<typename Policies>
        class PoliciesTraits
        {

            public:

                using DomainPolicy = typename Policies::DomainPolicy;

                using OutputsPolicies = typename Policies::OutputsPolicies;

                using InputsPolicies = typename Policies::InputsPolicies;
        };


        template<typename E>
        class TypeTraits<DetectNoDataByValue<E>>
        {

            public:

                using Element = E;

                template<typename E_>
                using Policy = std::conditional_t<
                    std::is_floating_point_v<E_>,
                    DetectNoDataByNaN<E_>,
                    DetectNoDataByValue<E_>>;
        };


        template<typename E>
        class TypeTraits<MarkNoDataByValue<E>>
        {

            public:

                using Element = E;

                template<typename E_>
                using Policy = std::
                    conditional_t<std::is_floating_point_v<E_>, MarkNoDataByNaN<E_>, MarkNoDataByValue<E_>>;

                using InputNoDataPolicy = DetectNoDataByValue<Element>;
        };


        template<typename E>
        class TypeTraits<DetectNoDataByNaN<E>>
        {

            public:

                using Element = E;

                template<typename E_>
                using Policy = std::conditional_t<
                    std::is_floating_point_v<E_>,
                    DetectNoDataByNaN<E_>,
                    DetectNoDataByValue<E_>>;
        };


        template<typename E>
        class TypeTraits<MarkNoDataByNaN<E>>
        {

            public:

                using Element = E;

                template<typename E_>
                using Policy = std::
                    conditional_t<std::is_floating_point_v<E_>, MarkNoDataByNaN<E_>, MarkNoDataByValue<E_>>;

                using InputNoDataPolicy = DetectNoDataByNaN<Element>;
        };

    }  // namespace detail


    template<typename Policies>
    using DomainPolicyT = typename detail::PoliciesTraits<Policies>::DomainPolicy;


    template<typename Policies>
    using OutputsPoliciesT = typename detail::PoliciesTraits<Policies>::OutputsPolicies;


    template<typename Policies>
    using InputsPoliciesT = typename detail::PoliciesTraits<Policies>::InputsPolicies;


    template<typename Policies>
    constexpr auto nr_inputs = Policies::nr_inputs;


    template<typename Policies>
    constexpr auto nr_outputs = Policies::nr_outputs;


    template<typename T>
    using ElementT = typename detail::TypeTraits<lue::detail::remove_cvref_t<T>>::Element;


    template<typename Policies, std::size_t idx>
    using PoliciesT =
        typename detail::TypeTraits<lue::detail::remove_cvref_t<Policies>>::template Policies<idx>;


    template<
        // All policies related to an operation
        typename Policies,
        // Idx of policies related to a specific input
        std::size_t idx = 0>
    using InputElementT = ElementT<PoliciesT<typename Policies::InputsPolicies, idx>>;


    template<
        // All policies related to an operation
        typename Policies,
        // Idx of policies related to a specific output
        std::size_t idx = 0>
    using OutputElementT = ElementT<PoliciesT<typename Policies::OutputsPolicies, idx>>;


    template<
        // All policies related to an operation
        typename Policies,
        // Idx of policies related to a specific output
        std::size_t idx,
        // Element for which to return an output policies type
        typename Element = OutputElementT<Policies, idx>>
    using OutputPoliciesT = typename detail::TypeTraits<
        // Policies related to a specific output
        PoliciesT<typename Policies::OutputsPolicies, idx>>::template Policies<Element>;


    template<
        // All policies related to an operation
        typename Policies,
        // Idx of policies related to a specific input
        std::size_t idx,
        // Element for which to return an input policies type
        typename Element = ElementT<PoliciesT<typename Policies::InputsPolicies, idx>>>
    using InputPoliciesT = typename detail::TypeTraits<
        // Policies related to a specific input
        PoliciesT<typename Policies::InputsPolicies, idx>>::template Policies<Element>;


    template<typename OutputPolicies>
    using OutputNoDataPolicy2T =
        typename detail::TypeTraits<lue::detail::remove_cvref_t<OutputPolicies>>::OutputNoDataPolicy;


    template<typename OutputNoDataPolicy, typename Element>
    using OutputNoDataPolicyT = typename detail::TypeTraits<
        lue::detail::remove_cvref_t<OutputNoDataPolicy>>::template Policy<Element>;


    template<
        // All policies related to an operation
        typename Policies,
        // Idx of policies related to a specific output
        std::size_t idx,
        // Element for which to return an output no-data policies type
        typename Element = ElementT<PoliciesT<typename Policies::OutputsPolicies, idx>>>
    using OutputNoDataPolicy3T = OutputNoDataPolicy2T<typename detail::TypeTraits<
        // Policies related to a specific output
        PoliciesT<typename Policies::OutputsPolicies, idx>>::template Policies<Element>>;


    // template<
    //     typename RangePolicy,
    //     typename Element>
    // using RangePolicyT =
    //     typename detail::TypeTraits<
    //             lue::detail::remove_cvref_t<RangePolicy>
    //         >::template Policy<Element>;


    template<typename InputPolicies>
    using InputNoDataPolicy2T =
        typename detail::TypeTraits<lue::detail::remove_cvref_t<InputPolicies>>::InputNoDataPolicy;


    template<typename InputNoDataPolicy, typename Element>
    using InputNoDataPolicyT =
        typename detail::TypeTraits<lue::detail::remove_cvref_t<InputNoDataPolicy>>::template Policy<Element>;


    template<typename HaloPolicy, typename Element>
    using HaloPolicyT =
        typename detail::TypeTraits<lue::detail::remove_cvref_t<HaloPolicy>>::template Policy<Element>;


    template<typename Element>
    inline constexpr Element no_data_value =
        detail::TypeTraits<lue::detail::remove_cvref_t<Element>>::no_data_value();

}  //  namespace lue::policy
