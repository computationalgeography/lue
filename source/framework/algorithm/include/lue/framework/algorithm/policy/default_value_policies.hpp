#pragma once
#include "lue/framework/algorithm/policy/policies.hpp"
#include "lue/framework/algorithm/policy/detect_no_data_by_nan.hpp"
#include "lue/framework/algorithm/policy/detect_no_data_by_value.hpp"
#include "lue/framework/algorithm/policy/fill_halo_with_constant_value.hpp"
#include "lue/framework/algorithm/policy/mark_no_data_by_nan.hpp"
#include "lue/framework/algorithm/policy/mark_no_data_by_value.hpp"
#include "lue/framework/algorithm/policy/input_policies.hpp"
#include "lue/framework/algorithm/policy/output_policies.hpp"
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include <hpx/serialization.hpp>
#include <array>


namespace lue::policy {

    template<
        typename Element>
    using DefaultInputNoDataPolicy =
        std::conditional_t<
            std::is_floating_point_v<Element>,
            DetectNoDataByNaN<Element>,
            DetectNoDataByValue<Element>
        >;


    template<
        typename Element>
    using DefaultOutputNoDataPolicy =
        std::conditional_t<
            std::is_floating_point_v<Element>,
            MarkNoDataByNaN<Element>,
            MarkNoDataByValue<Element>
        >;


    template<
        typename... Elements>
    using InputElements = detail::TypeList<Elements...>;


    template<
        typename... Elements>
    using OutputElements = detail::TypeList<Elements...>;


    template<
        typename DomainPolicy,
        typename OutputElements,
        typename InputElements>
    class DefaultValuePolicies
    {
    };


    /*!
        @brief      Default value policies to use by algorithms

        Defaults:
        - Output no-data policy marks no-data by NaN (floating point types) or value
        - Input no-data policy detects value by NaN (floating point types) of value
    */
    template<
        typename DomainPolicy,
        typename... OutputElements,
        typename... InputElements>
    class DefaultValuePolicies<
        DomainPolicy,
        detail::TypeList<OutputElements...>,
        detail::TypeList<InputElements...>>:

        public Policies<
            DomainPolicy,
            OutputsPolicies<OutputPolicies<DefaultOutputNoDataPolicy<OutputElements>>...>,
            InputsPolicies<InputPolicies<DefaultInputNoDataPolicy<InputElements>>...>>

    {
    };


    // namespace detail {

    //     template<
    //         typename DomainPolicy_,
    //         typename... OutputElements,
    //         typename... InputElements>
    //     class PoliciesTraits<
    //         DefaultValuePolicies<DomainPolicy_, OutputElements..., InputElements...>>
    //     {

    //         public:

    //             using DomainPolicy = DomainPolicy_;

    //     };

    // }  // namespace detail


    template<
        typename DomainPolicy,
        typename OutputElements,
        typename InputElements>
    class DefaultSpatialOperationValuePolicies
    {
    };


    template<
        typename DomainPolicy,
        typename... OutputElements,
        typename... InputElements>
    class DefaultSpatialOperationValuePolicies<
            DomainPolicy,
            detail::TypeList<OutputElements...>,
            detail::TypeList<InputElements...>>:

        public Policies<
            DomainPolicy,
            OutputsPolicies<OutputPolicies<DefaultOutputNoDataPolicy<OutputElements>>...>,
            InputsPolicies<
                SpatialOperationInputPolicies<
                    DefaultInputNoDataPolicy<InputElements>, FillHaloWithConstantValue<InputElements>>...>>

    {

        private:

            // MSVC requires that these templates are qualified by their namespaces
            using Base = Policies<
                DomainPolicy,
                lue::policy::OutputsPolicies<
                    lue::policy::OutputPolicies<
                        DefaultOutputNoDataPolicy<OutputElements>>...>,
                lue::policy::InputsPolicies<
                    SpatialOperationInputPolicies<
                        DefaultInputNoDataPolicy<InputElements>,
                        FillHaloWithConstantValue<InputElements>>...>>;

        public:

            DefaultSpatialOperationValuePolicies():

                Base{
                        DomainPolicy{},
                        OutputPolicies<
                                DefaultOutputNoDataPolicy<OutputElements>
                            >{}...,
                        SpatialOperationInputPolicies<
                                DefaultInputNoDataPolicy<InputElements>,
                                FillHaloWithConstantValue<InputElements>
                            >{FillHaloWithConstantValue<InputElements>{}}...
                    }

            {
            }


            DefaultSpatialOperationValuePolicies(
                InputElements const... fill_values):

                Base{
                        DomainPolicy{},
                        OutputPolicies<
                                DefaultOutputNoDataPolicy<OutputElements>
                            >{}...,
                        SpatialOperationInputPolicies<
                                DefaultInputNoDataPolicy<InputElements>,
                                FillHaloWithConstantValue<InputElements>
                            >{FillHaloWithConstantValue<InputElements>{fill_values}}...
                    }

            {
            }

    };

}  // namespace lue::policy
