#define BOOST_TEST_MODULE lue framework algorithm accu_threshold
#include "lue/framework/algorithm/accu_threshold.hpp"
#include "flow_accumulation.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

    // Grab symbols from flow_accumulation header
    using namespace lue::test;

    std::size_t const rank = 2;

    using MaterialElement = float;
    using MaterialArray = lue::PartitionedArray<MaterialElement, rank>;

    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        lue::Rank rank>
    void test_accu_threshold(
        Policies const& policies,
        lue::PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        lue::PartitionedArray<MaterialElement, rank> const& material,
        lue::PartitionedArray<MaterialElement, rank> const& threshold,
        lue::PartitionedArray<MaterialElement, rank> const& flux_we_want,
        lue::PartitionedArray<MaterialElement, rank> const& state_we_want)
    {
        auto [flux_we_got, state_we_got] = lue::accu_threshold(policies, flow_direction, material, threshold);

        lue::test::check_arrays_are_equal(flux_we_got, flux_we_want);
        lue::test::check_arrays_are_equal(state_we_got, state_we_want);
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(pcraster_manual_example1)
{
    using Policies = lue::policy::accu_threshold::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;
    Policies policies{};

    auto flow_direction = lue::test::pcraster_example_flow_direction();

    auto const array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    auto material = create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape, partition_shape,
        {{
                6.0, 0.5, 2.0, 2.0, 2.0,
                0.5, 0.5, 2.0, 2.0, 2.0,
                0.5, 0.5, 2.0, 2.0, 0.0,
                0.5, 0.5, 6.0, 0.0, 0.0,
                0.5, 6.0, 6.0, 6.0, 6.0,
        }});

    auto threshold = create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape, partition_shape,
        {{
                1.5, 1.5, 1.5, 1.5, 1.5,
                1.5, 1.5, 1.5, 1.5, 1.5,
                1.5, 1.5, 1.5, 1.5, 1.5,
                1.5, 1.5, 1.5, 1.5, 1.5,
                1.5, 1.5, 1.5, 1.5, 1.5,
        }});

    auto flux_we_want = create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape, partition_shape,
        {{
                4.5, 0.0, 0.5, 0.5, 0.5,
                3.5, 0.0, 1.5, 1.0, 0.5,
                2.5, 0.5, 2.5, 1.0, 0.0,
                0.0, 4.5, 4.5, 0.0, 0.0,
                0.0, 27.0, 13.5, 9.0, 4.5,
        }});

    auto state_we_want = create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape, partition_shape,
        {{
                1.5, 0.5, 1.5, 1.5, 1.5,
                1.5, 0.5, 1.5, 1.5, 1.5,
                1.5, 1.5, 1.5, 1.5, 0.0,
                0.5, 1.5, 1.5, 0.0, 0.0,
                0.5, 1.5, 1.5, 1.5, 1.5,
        }});

    test_accu_threshold(policies, flow_direction, material, threshold, flux_we_want, state_we_want);
}


BOOST_AUTO_TEST_CASE(pcraster_manual_example2)
{
    using Policies = lue::policy::accu_threshold::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;
    Policies policies{};

    auto flow_direction = lue::test::pcraster_example_flow_direction();

    auto const array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    auto material = create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape, partition_shape,
        {{
            10.0, 10.0, 10.0, 10.0, 10.0,
            10.0, 10.0, 10.0, 10.0, 10.0,
            10.0, 10.0, 10.0, 10.0, 10.0,
            10.0, 10.0, 10.0, 10.0, 50.0,
            50.0, 50.0, 50.0, 50.0, 49.0,
        }});

    auto threshold = create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape, partition_shape,
        {{
             0.0, 40.0,  9.0,  9.0,  9.0,
             0.0, 40.0,  9.0,  9.0,  9.0,
             0.0, 40.0, 30.0,  9.0,  9.0,
             0.0, 40.0,  9.0,  9.0, 50.0,
             0.0, 40.0, 40.0, 40.0, 50.0,
        }});

    auto flux_we_want = create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape, partition_shape,
        {{
            10.0,  0.0,  1.0,  1.0,  1.0,
            20.0,  0.0,  3.0,  2.0,  1.0,
            30.0,  0.0,  0.0,  2.0,  1.0,
            10.0,  0.0,  3.0,  2.0,  0.0,
            50.0, 93.0, 20.0, 10.0,  0.0,
        }});

    auto state_we_want = create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape, partition_shape,
        {{
            0.0, 10.0,  9.0,  9.0,  9.0,
            0.0, 10.0,  9.0,  9.0,  9.0,
            0.0, 13.0, 14.0,  9.0,  9.0,
            0.0, 40.0,  9.0,  9.0, 50.0,
            0.0, 40.0, 40.0, 40.0, 49.0,
        }});

    test_accu_threshold(policies, flow_direction, material, threshold, flux_we_want, state_we_want);
}
