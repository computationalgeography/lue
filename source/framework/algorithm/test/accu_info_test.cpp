#define BOOST_TEST_MODULE lue framework algorithm accu_info
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/value_policies/accu_info.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(overloads)
{
    using FlowDirectionElement = lue::FlowDirectionElement;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};

    [[maybe_unused]] lue::PartitionedArray<lue::CellClass, 2> class_ =
        lue::value_policies::accu_info(flow_direction);
}


BOOST_AUTO_TEST_CASE(all_no_data)
{
    auto const flow_direction = lue::test::all_no_data();

    lue::PartitionedArray<lue::CellClass, 2> const class_we_got =
        lue::value_policies::accu_info(flow_direction);

    auto const nd_class{lue::policy::no_data_value<lue::CellClass>};

    lue::PartitionedArray<lue::CellClass, 2> class_we_want = lue::create_partitioned_array<lue::CellClass, 2>(
        lue::test::array_shape, lue::test::partition_shape, nd_class);

    lue::test::check_arrays_are_equal(class_we_got, class_we_want);
}


BOOST_AUTO_TEST_CASE(spiral_in)
{
    auto const flow_direction = lue::test::spiral_in();

    lue::PartitionedArray<lue::CellClass, 2> const class_we_got =
        lue::value_policies::accu_info(flow_direction);

    auto const intra = lue::intra_partition_stream_cell;
    auto const ridge = lue::ridge_cell;
    auto const inter = lue::inter_partition_stream_cell;
    auto const input = lue::partition_input_cell;
    auto const output = lue::partition_output_cell;
    auto const input_output = lue::partition_input_output_cell;
    auto const sink = lue::sink_cell;

    auto const class_we_want = lue::test::create_partitioned_array<lue::PartitionedArray<lue::CellClass, 2>>(
        lue::test::array_shape,
        lue::test::partition_shape,
        // clang-format off
        {
            {
                ridge, intra, output,
                inter, inter, output,
                input, input, output,
            },
            {
                input, inter, output,
                input, inter, output,
                input, inter, output,
            },
            {
                input, inter, inter,
                input, inter, inter,
                input_output, output, output,
            },
            {
                output, output, output,
                inter, inter, inter,
                input, input, input,
            },
            {
                input, inter, inter,
                inter, sink, inter,
                inter, inter, inter,
            },
            {
                input, input, input,
                inter, inter, inter,
                output, output, output,
            },
            {
                output, output, input_output,
                inter, inter, input,
                inter, inter, input,
            },
            {
                output, inter, input,
                output, inter, input,
                output, inter, input,
            },
            {
                input_output, input, input,
                output, inter, inter,
                output, inter, inter,
            },
        }  // clang-format on
    );

    lue::test::check_arrays_are_equal(class_we_got, class_we_want);
}
