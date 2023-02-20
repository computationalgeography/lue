#define BOOST_TEST_MODULE lue framework algorithm upstream
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/definition/upstream.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

    // Grab symbols from flow_accumulation header
    using namespace lue::test;

    std::size_t const rank = 2;

    using MaterialElement = std::uint64_t;
    using MaterialArray = lue::PartitionedArray<MaterialElement, rank>;

    template<typename Policies, typename FlowDirectionElement, typename MaterialElement, lue::Rank rank>
    void test_upstream(
        Policies const& policies,
        lue::PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        lue::PartitionedArray<MaterialElement, rank> const& material,
        lue::PartitionedArray<MaterialElement, rank> const& upstream_we_want)
    {
        using MaterialArray = lue::PartitionedArray<MaterialElement, rank>;

        MaterialArray upstream_we_got = lue::upstream(policies, flow_direction, material);

        lue::test::check_arrays_are_equal(upstream_we_got, upstream_we_want);
    }


    template<typename FlowDirectionElement, typename MaterialElement, lue::Rank rank>
    void test_upstream(
        lue::PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        lue::PartitionedArray<MaterialElement, rank> const& material,
        lue::PartitionedArray<MaterialElement, rank> const& upstream_we_want)
    {
        using Policies = lue::policy::upstream::DefaultPolicies<FlowDirectionElement, MaterialElement>;

        test_upstream(Policies{}, flow_direction, material, upstream_we_want);
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(parallel_east)
{
    test_upstream(
        lue::test::create_partitioned_array<FlowDirectionArray>(
            array_shape,
            partition_shape,
            {
                {
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                },
                {
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                },
                {
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                },
                {
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                },
                {
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                },
                {
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                },
                {
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                },
                {
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                },
                {
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                    e,
                },
            }),
        lue::test::create_partitioned_array<MaterialArray>(
            array_shape,
            partition_shape,
            {
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                },
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                },
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                },
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                },
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                },
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                },
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                },
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                },
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                },
            }),
        lue::test::create_partitioned_array<MaterialArray>(
            array_shape,
            partition_shape,
            {
                {
                    0,
                    1,
                    1,
                    0,
                    1,
                    1,
                    0,
                    1,
                    1,
                },
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                },
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                },
                {
                    0,
                    1,
                    1,
                    0,
                    1,
                    1,
                    0,
                    1,
                    1,
                },
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                },
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                },
                {
                    0,
                    1,
                    1,
                    0,
                    1,
                    1,
                    0,
                    1,
                    1,
                },
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                },
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                },
            }));
}


BOOST_AUTO_TEST_CASE(pcraster_manual_example)
{
    using Shape = lue::ShapeT<FlowDirectionArray>;

    Shape array_shape{5, 5};
    Shape partition_shape{5, 5};

    using Policies = lue::policy::upstream::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;
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
                0, 0, 0, 0, 0, 1, 1, 4, 4, 0, 1, 3, x, 4, 0, 0, x, 4, 8, 0, 0, 14, 5, 5, 0,
            },
        });

    test_upstream(policies, flow_direction, material, result_we_want);
}
