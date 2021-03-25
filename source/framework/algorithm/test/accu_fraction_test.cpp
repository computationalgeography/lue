#define BOOST_TEST_MODULE lue framework algorithm accu_fraction
#include "lue/framework/algorithm/accu_fraction.hpp"
#include "flow_accumulation.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

    // Grab symbols from flow_accumulation header
    using namespace lue::test;

    std::size_t const rank = 2;

    using MaterialElement = std::uint64_t;
    using MaterialArray = lue::PartitionedArray<MaterialElement, rank>;

    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        lue::Rank rank>
    void test_accu_fraction(
        Policies const& policies,
        lue::PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        lue::PartitionedArray<MaterialElement, rank> const& material,
        lue::PartitionedArray<MaterialElement, rank> const& accu_we_want)
    {
        // using MaterialArray = lue::PartitionedArray<MaterialElement, rank>;

        // MaterialArray accu_we_got = lue::accu_fraction(policies, flow_direction, material);

        // lue::test::check_arrays_are_equal(accu_we_got, accu_we_want);
    }


    template<
        typename FlowDirectionElement,
        typename MaterialElement,
        lue::Rank rank>
    void test_accu_fraction(
        lue::PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        lue::PartitionedArray<MaterialElement, rank> const& material,
        lue::PartitionedArray<MaterialElement, rank> const& accu_we_want)
    {
        // using Policies = lue::policy::accu_fraction::DefaultPolicies<
        //     FlowDirectionElement, MaterialElement>;

        // test_accu_fraction(Policies{}, flow_direction, material, accu_we_want);
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(merging_streams_case_01)
{
    // // No-data material in ridge cell. From there on, it must propage
    // // down to all downstream cells, in all downstream partitions.

    // using Policies = lue::policy::accu_fraction::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;
    // Policies policies{};

    // auto flow_direction = lue::test::merging_streams();

    // auto const x{lue::policy::no_data_value<MaterialElement>};

    // auto material = create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
    //     array_shape, partition_shape,
    //     {
    //         { // 0, 0
    //             1, 1, 1,
    //             1, 1, 1,
    //             1, 1, 1,
    //         },
    //         { // 0, 1
    //             1, 1, 1,
    //             1, 1, 1,
    //             1, 1, 1,
    //         },
    //         { // 0, 2
    //             1, x, 1,
    //             1, 1, 1,
    //             1, 1, 1,
    //         },
    //         { // 1, 0
    //             1, 1, 1,
    //             1, 1, 1,
    //             1, 1, 1,
    //         },
    //         { // 1, 1
    //             1, 1, 1,
    //             1, 1, 1,
    //             1, 1, 1,
    //         },
    //         { // 1, 2
    //             1, 1, 1,
    //             1, 1, 1,
    //             1, 1, 1,
    //         },
    //         { // 2, 0
    //             1, 1, 1,
    //             1, 1, 1,
    //             1, 1, 1,
    //         },
    //         { // 2, 1
    //             1, 1, 1,
    //             1, 1, 1,
    //             1, 1, 1,
    //         },
    //         { // 2, 2
    //             1, 1, 1,
    //             1, 1, 1,
    //             1, 1, 1,
    //         },
    //     });

    // auto result_we_want = create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
    //     array_shape, partition_shape,
    //     {
    //         { // 0, 0
    //             x, x, x,
    //             x, x, x,
    //             x, x, 1,
    //         },
    //         { // 0, 1
    //             x, 1, 1,
    //             x, 1, 1,
    //             x, 2, x,
    //         },
    //         { // 0, 2
    //             x, x, 1,
    //             3, x, 1,
    //             x, x, 1,
    //         },
    //         { // 1, 0
    //             x, x, 1,
    //             x, x, 1,
    //             x, x, 1,
    //         },
    //         { // 1, 1
    //             4, x, x,
    //             x, x, 2,
    //             2, x, 3,
    //         },
    //         { // 1, 2
    //             x, x, x,
    //             1, x, x,
    //             1, x, x,
    //         },
    //         { // 2, 0
    //             x, x, x,
    //             x, x, x,
    //             x, x, x,
    //         },
    //         { // 2, 1
    //             x, x, x,
    //             x, x, 2,
    //             4, 3, 2,
    //         },
    //         { // 2, 2
    //             1, x, x,
    //             1, x, x,
    //             1, x, x,
    //         },
    //     });

    // test_accu(policies, flow_direction, material, result_we_want);
}
