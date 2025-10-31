#pragma once
#include "lue/framework/algorithm/focal_diversity.hpp"


namespace lue {
    namespace policy::focal_diversity {

        template<typename Count>
        using DefaultOutputPolicies =
            policy::OutputPolicies<DontMarkNoData<Count>, AllValuesWithinRange<Count>>;


        template<typename Element>
        using DefaultInputPolicies =
            policy::SpatialOperationInputPolicies<SkipNoData<Element>, FillHaloWithConstantValue<Element>>;


        template<typename Count, typename Element>
        using DefaultPolicies = policy::Policies<
            DomainPolicy<Element>,
            OutputsPolicies<DefaultOutputPolicies<Count>>,
            InputsPolicies<DefaultInputPolicies<Element>>>;

    }  // namespace policy::focal_diversity


    namespace default_policies {

        template<typename Count, typename Element, Rank rank, typename Kernel>
        auto focal_diversity(PartitionedArray<Element, rank> const& array, Kernel const& kernel)
            -> PartitionedArray<Count, rank>
        {
            using Policies = policy::focal_diversity::DefaultPolicies<Count, Element>;

            Element const fill_value{0};

            policy::focal_diversity::DomainPolicy<Element> domain_policy{};
            policy::focal_diversity::DefaultOutputPolicies<Count> output_policies{};
            policy::focal_diversity::DefaultInputPolicies<Element> input_policies{
                policy::FillHaloWithConstantValue<Element>{fill_value}};
            policy::focal_diversity::DefaultPolicies<Count, Element> policies{
                domain_policy, output_policies, input_policies};

            return focal_diversity(policies, array, kernel);
        }

    }  // namespace default_policies
}  // namespace lue
