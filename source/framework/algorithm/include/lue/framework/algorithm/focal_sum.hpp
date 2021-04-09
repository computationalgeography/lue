#pragma once
#include "lue/framework/algorithm/focal_operation.hpp"
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class FocalSum
{

public:

    using OutputElement = InputElement;

    FocalSum()=default;

    template<
        typename Kernel,
        typename OutputPolicies,
        typename InputPolicies,
        typename Subspan>
    OutputElement operator()(
        Kernel const& kernel,
        [[maybe_unused]] OutputPolicies const& output_policies,
        [[maybe_unused]] InputPolicies const& input_policies,
        Subspan const& window) const
    {
        static_assert(rank<Kernel> == 2);

        using Weight = ElementT<Kernel>;

        // TODO Add traits to grab typename of elements in Subspan
        // static_assert(std::is_same_v<ElementT<Subspan>, InputElement>);
        static_assert(std::is_same_v<Weight, bool> || std::is_floating_point_v<Weight>);

        OutputElement sum{0};

        lue_hpx_assert(window.extent(0) == kernel.size());
        lue_hpx_assert(window.extent(1) == kernel.size());

        for(Index r = 0; r < window.extent(0); ++r) {
            for(Index c = 0; c < window.extent(1); ++c)
            {
                Weight const weight{kernel(r, c)};
                InputElement const value{window(r, c)};

                if constexpr(std::is_same_v<Weight, bool>)
                {
                    // TODO Handle no-data
                    if(weight)
                    {
                        sum += value;
                    }
                }
                else
                {
                    // TODO Handle no-data
                    sum += weight * value;
                }
            }
        }

        return sum;
    }

};

}  // namespace detail


namespace policy::focal_sum {

    template<
        typename OutputElement,
        typename InputElement>
    using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
        AllValuesWithinDomain<InputElement>,
        OutputElements<OutputElement>,
        InputElements<InputElement>>;

}  // namespace focal_sum::policy


template<
    typename Element,
    Rank rank,
    typename Kernel>
PartitionedArray<Element, rank> focal_sum(
    PartitionedArray<Element, rank> const& array,
    Kernel const& kernel)
{
    using Functor = detail::FocalSum<Element>;
    using OutputElement = OutputElementT<Functor>;
    using InputElement = Element;
    using Policies = policy::focal_sum::DefaultPolicies<OutputElement, InputElement>;

    InputElement const fill_value{0};

    return focal_operation(Policies{fill_value}, array, kernel, Functor{});
}

}  // namespace lue
