#define BOOST_TEST_MODULE lue framework algorithm accu_fraction
#include "lue/framework/algorithm/definition/accu_fraction.hpp"
#include "flow_accumulation.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

    // Grab symbols from flow_accumulation header
    using namespace lue::test;

    std::size_t const rank = 2;

    using FractionElement = float;
    using FractionArray = lue::PartitionedArray<FractionElement, rank>;

    using MaterialElement = FractionElement;
    using MaterialArray = lue::PartitionedArray<MaterialElement, rank>;

    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        typename FractionElement,
        lue::Rank rank>
    void test_accu_fraction(
        Policies const& policies,
        lue::PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        lue::PartitionedArray<MaterialElement, rank> const& material,
        lue::PartitionedArray<FractionElement, rank> const& fraction,
        lue::PartitionedArray<MaterialElement, rank> const& flux_we_want,
        lue::PartitionedArray<MaterialElement, rank> const& state_we_want)
    {
        auto [flux_we_got, state_we_got] = lue::accu_fraction(policies, flow_direction, material, fraction);

        lue::test::check_arrays_are_equal(flux_we_got, flux_we_want);
        lue::test::check_arrays_are_equal(state_we_got, state_we_want);
    }


    template<
        typename FlowDirectionElement,
        typename MaterialElement,
        typename FractionElement,
        lue::Rank rank>
    void test_accu_fraction(
        lue::PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        lue::PartitionedArray<MaterialElement, rank> const& material,
        lue::PartitionedArray<FractionElement, rank> const& fraction,
        lue::PartitionedArray<MaterialElement, rank> const& flux_we_want,
        lue::PartitionedArray<MaterialElement, rank> const& state_we_want)
    {
        using Policies = lue::policy::accu_fraction::DefaultPolicies<
            FlowDirectionElement, MaterialElement, FractionElement>;

        test_accu_fraction(Policies{}, flow_direction, material, fraction, flux_we_want, state_we_want);
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(spiral_in_case)
{
    // accu_fraction(flow_direction, material, ones) == accu(flow_direction, material)

    auto flow_direction = lue::test::spiral_in();
    auto material = lue::test::ones<MaterialElement>();
    auto fraction = lue::test::ones<FractionElement>();
    auto flux_we_want = lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
        {
            {  1,  2,  3,
              32, 33, 34,
              31, 56, 57, },
            {  4,  5,  6,
              35, 36, 37,
              58, 59, 60, },
            {  7,  8,  9,
              38, 39, 10,
              61, 40, 11, },
            { 30, 55, 72,
              29, 54, 71,
              28, 53, 70, },
            { 73, 74, 75,
              80, 81, 76,
              79, 78, 77, },
            { 62, 41, 12,
              63, 42, 13,
              64, 43, 14, },
            { 27, 52, 69,
              26, 51, 50,
              25, 24, 23, },
            { 68, 67, 66,
              49, 48, 47,
              22, 21, 20, },
            { 65, 44, 15,
              46, 45, 16,
              19, 18, 17, },
        });

    auto state_we_want = lue::test::zeros<FractionElement>();

    test_accu_fraction(flow_direction, material, fraction, flux_we_want, state_we_want);
}


BOOST_AUTO_TEST_CASE(merging_streams_case_01)
{
    // No-data material in ridge cell. From there on, it must propagate
    // down to all downstream cells, in all downstream partitions.
    // Similar for a no-data fraction in ridge cell.

    using Policies =
        lue::policy::accu_fraction::DefaultValuePolicies<FlowDirectionElement, MaterialElement, FractionElement>;
    Policies policies{};

    auto flow_direction = lue::test::merging_streams();

    auto const x{lue::policy::no_data_value<MaterialElement>};

    auto material = create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape, partition_shape,
        {
            { // 0, 0
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 0, 1
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 0, 2
                1, x, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 1, 0
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 1, 1
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 1, 2
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 2, 0
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 2, 1
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 2, 2
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
        });

    auto fraction = create_partitioned_array<lue::PartitionedArray<FractionElement, 2>>(
        array_shape, partition_shape,
        {
            { // 0, 0
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            { // 0, 1
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            { // 0, 2
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            { // 1, 0
                0.5, 0.5, 0.5,
                0.5, 0.5,  x ,
                0.5, 0.5, 0.5,
            },
            { // 1, 1
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            { // 1, 2
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            { // 2, 0
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            { // 2, 1
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            { // 2, 2
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
        });

    auto flux_we_want = create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape, partition_shape,
        {
            { // 0, 0
                   x,    x,    x,
                   x,    x,    x,
                   x,    x, 0.50,
            },
            { // 0, 1
                   x, 0.50, 0.50,
                   x, 0.50, 0.50,
                   x, 0.75,    x,
            },
            { // 0, 2
                   x,    x, 0.50,
                1.00,    x, 0.50,
                   x,    x, 0.50,
            },
            { // 1, 0
                   x,    x, 0.50,
                   x,    x,    x,
                   x,    x, 0.50,
            },
            { // 1, 1
                   x,    x,    x,
                   x,    x, 0.75,
                0.75,    x, 1,
            },
            { // 1, 2
                x   ,    x,    x,
                0.50,    x,    x,
                0.50,    x,    x,
            },
            { // 2, 0
                   x,    x,    x,
                   x,    x,    x,
                   x,    x,    x,
            },
            { // 2, 1
                   x,    x,    x,
                   x,    x, 0.75,
                0.9375, 0.875, 0.75,
            },
            { // 2, 2
                0.50,    x,    x,
                0.50,    x,    x,
                0.50,    x,    x,
            },
        });

    auto state_we_want = create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape, partition_shape,
        {
            { // 0, 0
                   x,    x,    x,
                   x,    x,    x,
                   x,    x,  0.5,
            },
            { // 0, 1
                   x, 0.50, 0.50,
                   x, 0.50, 0.50,
                   x, 0.75,    x,
            },
            { // 0, 2
                   x,    x, 0.50,
                1.00,    x, 0.50,
                   x,    x, 0.50,
            },
            { // 1, 0
                   x,    x, 0.50,
                   x,    x,    x,
                   x,    x, 0.50,
            },
            { // 1, 1
                   x,    x,    x,
                   x,    x, 0.75,
                0.75,    x,    1,
            },
            { // 1, 2
                   x,    x,    x,
                0.50,    x,    x,
                0.50,    x,    x,
            },
            { // 2, 0
                   x,    x,    x,
                   x,    x,    x,
                   x,    x,    x,
            },
            { // 2, 1
                   x,    x,    x,
                   x,    x, 0.75,
                0.9375, 0.875, 0.75,
            },
            { // 2, 2
                0.50,    x,    x,
                0.50,    x,    x,
                0.50,    x,    x,
            },
        });

    test_accu_fraction(policies, flow_direction, material, fraction, flux_we_want, state_we_want);
}
