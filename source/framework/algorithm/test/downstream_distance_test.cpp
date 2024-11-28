#define BOOST_TEST_MODULE lue framework algorithm downstream_distance
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/definition/downstream_distance.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


namespace {

    // Grab symbols from flow_accumulation header
    using namespace lue::test;

    std::size_t const rank = 2;

    using DistanceElement = lue::FloatingPointElement<0>;
    using DistanceArray = lue::PartitionedArray<DistanceElement, rank>;

    template<typename Policies, typename FlowDirectionElement, typename DistanceElement, lue::Rank rank>
    void test_downstream_distance(
        Policies const& policies,
        lue::PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        DistanceElement const cell_size,
        lue::PartitionedArray<DistanceElement, rank> const& downstream_distance_we_want)
    {
        lue::PartitionedArray<DistanceElement, rank> downstream_distance_we_got =
            lue::downstream_distance(policies, flow_direction, cell_size);

        lue::test::check_arrays_are_equal(downstream_distance_we_got, downstream_distance_we_want);
    }


    template<typename FlowDirectionElement, typename DistanceElement, lue::Rank rank>
    void test_downstream_distance(
        lue::PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        DistanceElement const cell_size,
        lue::PartitionedArray<DistanceElement, rank> const& downstream_distance_we_want)
    {
        using Policies =
            lue::policy::downstream_distance::DefaultPolicies<FlowDirectionElement, DistanceElement>;

        test_downstream_distance(Policies{}, flow_direction, cell_size, downstream_distance_we_want);
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(pcraster_manual_example)
{
    using Shape = lue::ShapeT<FlowDirectionArray>;

    Shape array_shape{5, 5};
    Shape partition_shape{5, 5};

    using Policies =
        lue::policy::downstream_distance::DefaultValuePolicies<FlowDirectionElement, DistanceElement>;
    Policies policies{};

    DistanceElement const cell_size{2};
    auto const q{std::sqrt(DistanceElement{2}) * cell_size};

    auto flow_direction = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            {
                s, s, s, sw, sw, s, s, sw, sw, sw, se, s, sw, w, sw, se, s, sw, w, w, e, p, w, w, w,
            },
        });
    auto result_we_want = lue::test::create_partitioned_array<DistanceArray>(
        array_shape,
        partition_shape,
        {
            {
                2, 2, 2, q, q, 2, 2, q, q, q, q, 2, q, 2, q, q, 2, q, 2, 2, 2, 0, 2, 2, 2,
            },
        });

    test_downstream_distance(policies, flow_direction, cell_size, result_we_want);
}
