#pragma once
#include "lue/framework/algorithm/policy/policies.hpp"
#include "lue/framework/algorithm/policy/dont_mark_no_data.hpp"
#include "lue/framework/algorithm/policy/fill_halo_with_constant_value.hpp"
#include "lue/framework/algorithm/policy/input_policies.hpp"
#include "lue/framework/algorithm/policy/output_policies.hpp"
#include "lue/framework/algorithm/policy/skip_no_data.hpp"
#include <hpx/serialization.hpp>
#include <array>


namespace lue::policy {

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
    class DefaultPolicies
    {
    };


    /*!
        @brief      Default policies to use by algorithms

        Defaults:
        - Output no-data policy does not mark no-data in the output
        - Input no-data policy does not check for no-data in the input
    */
    template<
        typename DomainPolicy,
        typename... OutputElements,
        typename... InputElements>
    class DefaultPolicies<
        DomainPolicy,
        detail::TypeList<OutputElements...>,
        detail::TypeList<InputElements...>>:

        public Policies<
            DomainPolicy,
            OutputsPolicies<OutputPolicies<DontMarkNoData<OutputElements>>...>,
            InputsPolicies<InputPolicies<SkipNoData<InputElements>>...>>

    {
    };


    template<
        typename DomainPolicy,
        typename OutputElements,
        typename InputElements>
    class DefaultSpatialOperationPolicies
    {
    };


    /*!
        @brief      Default policies to use by spatial operations

        In this context, spatial operations are operations with dependencies
        between elements from multiple partitions. Such operations use a
        halo of partitions around the input partitioned array(s). This class
        has input policies that have a policy for filling such a halo.

        Defaults:
        - Outputs:
           - No-data policy does not mark no-data in the output
        - Inputs:
           - No-data policy does not check for no-data in the input
           - Halos are filled with a constant value
    */
    template<
        typename DomainPolicy,
        typename... OutputElements,
        typename... InputElements>
    class DefaultSpatialOperationPolicies<
            DomainPolicy,
            detail::TypeList<OutputElements...>,
            detail::TypeList<InputElements...>>:

        public Policies<
            DomainPolicy,
            OutputsPolicies<OutputPolicies<DontMarkNoData<OutputElements>>...>,
            InputsPolicies<
                SpatialOperationInputPolicies<
                    SkipNoData<InputElements>, FillHaloWithConstantValue<InputElements>>...>>

    {

        private:

            // MSVC requires that these templates are qualified by their namespaces
            using Base = Policies<
                DomainPolicy,
                lue::policy::OutputsPolicies<
                    lue::policy::OutputPolicies<
                        DontMarkNoData<OutputElements>>...>,
                lue::policy::InputsPolicies<
                    SpatialOperationInputPolicies<
                        SkipNoData<InputElements>,
                        FillHaloWithConstantValue<InputElements>>...>>;

        public:

            DefaultSpatialOperationPolicies():

                Base{
                        DomainPolicy{},
                        OutputPolicies<
                                DontMarkNoData<OutputElements>
                            >{}...,
                        SpatialOperationInputPolicies<
                                SkipNoData<InputElements>,
                                FillHaloWithConstantValue<InputElements>
                            >{FillHaloWithConstantValue<InputElements>{}}...
                    }

            {
            }


            DefaultSpatialOperationPolicies(
                InputElements const... fill_values):

                Base{
                        DomainPolicy{},
                        OutputPolicies<
                                DontMarkNoData<OutputElements>
                            >{}...,
                        SpatialOperationInputPolicies<
                                SkipNoData<InputElements>,
                                FillHaloWithConstantValue<InputElements>
                            >{FillHaloWithConstantValue<InputElements>{fill_values}}...
                    }

            {
            }

    };

}  // namespace lue::policy
