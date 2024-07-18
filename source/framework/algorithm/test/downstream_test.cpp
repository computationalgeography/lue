#define BOOST_TEST_MODULE lue framework algorithm downstream
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/definition/downstream.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

    // Grab symbols from flow_accumulation header
    using namespace lue::test;

    std::size_t const rank = 2;

    using MaterialElement = std::uint64_t;
    using MaterialArray = lue::PartitionedArray<MaterialElement, rank>;

    template<typename Policies, typename FlowDirectionElement, typename MaterialElement, lue::Rank rank>
    void test_downstream(
        Policies const& policies,
        lue::PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        lue::PartitionedArray<MaterialElement, rank> const& material,
        lue::PartitionedArray<MaterialElement, rank> const& downstream_we_want)
    {
        using MaterialArray = lue::PartitionedArray<MaterialElement, rank>;

        MaterialArray downstream_we_got = lue::downstream(policies, flow_direction, material);

        lue::test::check_arrays_are_equal(downstream_we_got, downstream_we_want);
    }


    template<typename FlowDirectionElement, typename MaterialElement, lue::Rank rank>
    void test_downstream(
        lue::PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        lue::PartitionedArray<MaterialElement, rank> const& material,
        lue::PartitionedArray<MaterialElement, rank> const& downstream_we_want)
    {
        using Policies = lue::policy::downstream::DefaultPolicies<FlowDirectionElement, MaterialElement>;

        test_downstream(Policies{}, flow_direction, material, downstream_we_want);
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(pcraster_manual_example)
{
    using Shape = lue::ShapeT<FlowDirectionArray>;

    Shape array_shape{5, 5};
    Shape partition_shape{5, 5};

    using Policies = lue::policy::downstream::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;
    Policies policies{};

    auto const x{lue::policy::no_data_value<MaterialElement>};

    auto flow_direction = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            {
                s, s, s, sw, sw, s, s, sw, sw, sw, se, s, sw, w, sw, se, s, sw, w, w, e, p, w, w, w,
            },
        });
    auto material = lue::test::create_partitioned_array<MaterialArray>(
        array_shape,
        partition_shape,
        {
            {
                1, 1, 2, 2, 4, 1, 1, 2, 2, 4, 2, 2, x, 4, 4, 2, 2, 2, 4, 4, 3, 7, 5, 5, 5,
            },
        });
    auto result_we_want = lue::test::create_partitioned_array<MaterialArray>(
        array_shape,
        partition_shape,
        {
            {
                1, 1, 2, 2, 2, 2, 2, 2, x, 4, 2, 2, x, x, 4, 7, 7, 7, 2, 4, 7, 7, 7, 5, 5,
            },
        });

    test_downstream(policies, flow_direction, material, result_we_want);
}
