#pragma once
#include "lue/framework/algorithm/focal_operation.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include <type_traits>


namespace lue {
namespace detail {

template<
    typename FlowDirectionElement,
    typename ElevationElement>
class D8Network
{

public:

    using OutputElement = FlowDirectionElement;

    D8Network()=default;

    template<
        typename Kernel,
        typename Subspan>
    FlowDirectionElement operator()(
        Kernel const& kernel,
        Subspan const& window) const
    {
        using Weight = ElementT<Kernel>;

        // TODO Add traits to grab typename of elements in Subspan
        // static_assert(std::is_same_v<ElementT<Subspan>, ElevationElement>);
        static_assert(std::is_convertible_v<Weight, bool>);

        // We are assuming a 3x3 kernel, containing only true weights
        static_assert(rank<Kernel> == 2);
        lue_assert(kernel.size() == 3);
        lue_assert(std::all_of(kernel.begin(), kernel.end(), [](auto w) { return bool{w}; }));

        lue_assert(window.extent(0) == kernel.size());
        lue_assert(window.extent(1) == kernel.size());

        ElevationElement drop{0};
        FlowDirectionElement direction{5};

        ElevationElement other_drop;

        // First try the nearest cells -----------------------------------------
        // North
        other_drop = window(1, 1) - window(0, 1);
        if(other_drop > drop)
        {
            drop = other_drop;
            direction = 8;
        }

        // West
        other_drop = window(1, 1) - window(1, 0);
        if(other_drop > drop)
        {
            drop = other_drop;
            direction = 4;
        }

        // East
        other_drop = window(1, 1) - window(1, 2);
        if(other_drop > drop)
        {
            drop = other_drop;
            direction = 6;
        }

        // South
        other_drop = window(1, 1) - window(2, 1);
        if(other_drop > drop)
        {
            drop = other_drop;
            direction = 2;
        }

        // Move on the diagonal cells ------------------------------------------
        // North-west
        other_drop = window(1, 1) - window(0, 0);
        if(other_drop > drop)
        {
            drop = other_drop;
            direction = 7;
        }

        // North-east
        other_drop = window(1, 1) - window(0, 2);
        if(other_drop > drop)
        {
            drop = other_drop;
            direction = 9;
        }

        // South-west
        other_drop = window(1, 1) - window(2, 0);
        if(other_drop > drop)
        {
            drop = other_drop;
            direction = 1;
        }

        // South-east
        other_drop = window(1, 1) - window(2, 2);
        if(other_drop > drop)
        {
            drop = other_drop;
            direction = 3;
        }

        return direction;
    }

};

}  // namespace detail


namespace policy {
namespace d8 {

template<
    typename OutputElement,
    typename InputElement>
using DefaultPolicies = policy::DefaultFocalOperationPolicies<
    OutputElements<OutputElement>,
    InputElements<InputElement>>;

}  // namespace d8
}  // namespace policy


template<
    typename FlowDirectionElement,
    typename ElevationElement,
    Rank rank>
PartitionedArray<FlowDirectionElement, rank> d8_network(
    PartitionedArray<ElevationElement, rank> const& elevation)
{
    using Weight = bool;
    using Functor = detail::D8Network<FlowDirectionElement, ElevationElement>;
    using Policies = policy::d8::DefaultPolicies<FlowDirectionElement, ElevationElement>;

    ElevationElement const fill_value{std::numeric_limits<ElevationElement>::min()};
    Kernel<Weight, rank> kernel{box_kernel<Weight, rank>(1, true)};

    return focal_operation(Policies{fill_value}, elevation, kernel, Functor{});
}

}  // namespace lue
