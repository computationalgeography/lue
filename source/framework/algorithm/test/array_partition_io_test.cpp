#define BOOST_TEST_MODULE lue framework algorithm array_partition_io
#include "lue/framework/algorithm/array_partition_io.hpp"
#include "lue/framework/test/stream.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    lue::ArrayPartitionIO<lue::Index, 2, float> partition_io{};
    BOOST_CHECK_EQUAL(partition_io.nr_output_cells(), 0);
}


BOOST_AUTO_TEST_CASE(use_case_01)
{
    using Value = float;
    using ArrayPartitionIO = lue::ArrayPartitionIO<lue::Index, 2, float>;
    using Indices = ArrayPartitionIO::Indices;
    using CellsIdxs = ArrayPartitionIO::CellsIdxs;

    ArrayPartitionIO partition_io{};

    Indices output_cell_idx{4};
    CellsIdxs input_cell_idxs{{1, 1}, {3, 3}, {5, 5}, {7, 7}, {9, 9}};
    Value value{};

    // Copy arguments, instead of move. We need them for testing.
    partition_io.add(output_cell_idx, CellsIdxs{input_cell_idxs}, Value{value});

    BOOST_CHECK_EQUAL(partition_io.nr_output_cells(), 1);
    BOOST_REQUIRE_EQUAL(partition_io.input_cells(output_cell_idx).size(), input_cell_idxs.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(
        partition_io.input_cells(output_cell_idx).begin(), partition_io.input_cells(output_cell_idx).end(),
        input_cell_idxs.begin(), input_cell_idxs.end());
    BOOST_CHECK_EQUAL(partition_io.value(output_cell_idx), value);

    partition_io.remove_input_cell(output_cell_idx, {5, 5});
    input_cell_idxs.erase(input_cell_idxs.begin() + 2);
    BOOST_REQUIRE_EQUAL(partition_io.input_cells(output_cell_idx).size(), input_cell_idxs.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(
        partition_io.input_cells(output_cell_idx).begin(), partition_io.input_cells(output_cell_idx).end(),
        input_cell_idxs.begin(), input_cell_idxs.end());

    value = 9.9;
    partition_io.update_value(output_cell_idx, Value{value});
    BOOST_CHECK_EQUAL(partition_io.value(output_cell_idx), value);
}
