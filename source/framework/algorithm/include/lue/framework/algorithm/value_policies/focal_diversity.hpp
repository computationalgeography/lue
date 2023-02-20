#pragma once
#include "lue/framework/algorithm/focal_diversity.hpp"
#include <limits>


namespace lue {
    namespace policy::focal_diversity {

        template<typename Count>
        using DefaultValueOutputPolicies =
            policy::OutputPolicies<DefaultOutputNoDataPolicy<Count>, AllValuesWithinRange<Count>>;


        template<typename Element>
        using DefaultValueInputPolicies = policy::
            SpatialOperationInputPolicies<DetectNoDataByValue<Element>, FillHaloWithConstantValue<Element>>;


        template<typename Count, typename Element>
        using DefaultValuePolicies = policy::Policies<
            DomainPolicy<Element>,
            OutputsPolicies<DefaultValueOutputPolicies<Count>>,
            InputsPolicies<DefaultValueInputPolicies<Element>>>;

    }  // namespace policy::focal_diversity


    namespace value_policies {

        template<typename Count, typename Element, Rank rank, typename Kernel>
        PartitionedArray<Count, rank> focal_diversity(
            PartitionedArray<Element, rank> const& array, Kernel const& kernel)
        {
            // TODO This one should be policy-based
            Element const fill_value{policy::no_data_value<Element>};

            policy::focal_diversity::DomainPolicy<Element> domain_policy{};
            policy::focal_diversity::DefaultValueOutputPolicies<Count> output_policies{};
            policy::focal_diversity::DefaultValueInputPolicies<Element> input_policies{
                policy::FillHaloWithConstantValue<Element>{fill_value}};
            policy::focal_diversity::DefaultValuePolicies<Count, Element> policies{
                domain_policy, output_policies, input_policies};

            return focal_diversity<Count>(policies, array, kernel);
        }

    }  // namespace value_policies
}  // namespace lue
