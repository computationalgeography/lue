#define BOOST_TEST_MODULE lue framework algorithm routing_algorithm


// #include "flow_accumulation.hpp"
// #include "lue/framework/algorithm/definition/upstream.hpp"

#include "lue/framework/test/hpx_unit_test.hpp"
// #include "lue/framework.hpp"

#include "lue/framework/partitioned_array.hpp"
#include "lue/concept.hpp"


namespace lue {

    template<
        typename... Results,
        typename Policies,
        typename Functor,
        typename FlowDirectionElement,
        typename... Arguments>
    Results routing_algorithm(
        [[maybe_unused]] Policies const& policies,
        [[maybe_unused]] Functor&& functor,
        [[maybe_unused]] PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        [[maybe_unused]] Arguments const&... arguments)
    {
        // Do whatever it takes to end up with correct results

        // TODO: Return a single result in case the number of result types is one, and a tuple of results in
        // case the number of result types is larger than one.

        // TODO: There is an interaction between the functor and the number of results and their type
        // A result can be a spatial or a scalar (aggregate)


        return Results... {};
    }


    template<typename Policies>
    class MyFunctor
    {
    };


    template<typename Policies, typename FlowDirectionElement>
    PartitionedArray<float, 2> my_router(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<float, 2> const& array)
    {
        using Functor = MyFunctor<Policies>;

        return routing_algorithm<PartitionedArray<float, 2>>(policies, Functor{}, flow_direction, array);
    }


    template<typename Policies, typename FlowDirectionElement>
    float my_router(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        float const& value)
    {
        using Functor = MyFunctor<Policies>;

        return routing_algorithm<float>(policies, Functor{}, flow_direction, value);
    }

}  // namespace lue


BOOST_AUTO_TEST_CASE(overload)
{
    using Policies = int;
    using FlowDirectionElement = std::uint8_t;
    using Element = float;

    Policies const policies{};
    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};

    {
        lue::PartitionedArray<Element, 2> const arg{};
        [[maybe_unused]] lue::PartitionedArray<Element, 2> material =
            my_router(policies, flow_direction, arg);
    }

    {
        Element const arg{};
        [[maybe_unused]] Element result = my_router(policies, flow_direction, arg);
    }
}
