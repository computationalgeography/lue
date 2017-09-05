#define BOOST_TEST_MODULE lue hdf5 dataset
#include <boost/test/unit_test.hpp>
#include "lue/hdf5/dataset.hpp"
#include "lue/hdf5/file.hpp"


using namespace lue::hdf5;


struct Read2DDatasetFixture
{

    Read2DDatasetFixture()

        : nr_rows{3},
          nr_cols{2},
          nr_cells{nr_rows * nr_cols},
          memory_datatype{H5T_NATIVE_INT32}

    {
        // 2D, no chunking
        // +----+----+
        // | -2 | -1 |
        // +----+----+
        // |  0 |  9 |
        // +----+----+
        // |  1 |  2 |
        // +----+----+
        auto file = create_in_memory_file("my_dataset.lue");
        Dataset::CreationPropertyList creation_property_list;
        std::string const dataset_name = "my_dataset";
        Datatype const file_datatype = H5T_STD_I32LE;
        Dataspace const dataspace = create_dataspace({nr_rows, nr_cols});
        dataset = std::make_unique<Dataset>(
            lue::hdf5::create_dataset(
                file.id(), dataset_name, file_datatype, dataspace,
                creation_property_list));

        values = {
            -2, -1,
             0,  9,
             1,  2
        };

        dataset->write(memory_datatype, values.data());
    }


    void test_read_row(
        size_t const row)
    {
        std::vector<std::int32_t> values_read(nr_cols, 555);

        Offset const start{row, 0};
        Stride const stride{1, 1};
        Count const count{1, nr_cols};

        dataset->read(memory_datatype, Hyperslab(start, stride, count),
            values_read.data());

        BOOST_CHECK_EQUAL_COLLECTIONS(
            values_read.begin(), values_read.end(),
            values.begin() + row * nr_cols,
            values.begin() + (row + 1 ) * nr_cols);
    }


    void test_read_col(
        size_t const col)
    {
        std::vector<std::int32_t> values_read(nr_rows, 555);

        Offset const start{0, col};
        Stride const stride{1, 1};
        Count const count{nr_rows, 1};

        dataset->read(memory_datatype, Hyperslab(start, stride, count),
            values_read.data());

        for(size_t r = 0; r < nr_rows; ++r) {
            BOOST_CHECK_EQUAL(values_read[r], values[r * nr_cols + col]);
        }
    }


    void test_read_cell(
        size_t const row,
        size_t const col)
    {
        std::int32_t value_read{555};

        Offset const start{row, col};
        Stride const stride{1, 1};
        Count const count{1, 1};

        dataset->read(memory_datatype, Hyperslab(start, stride, count),
            &value_read);

        BOOST_CHECK_EQUAL(value_read, values[row * nr_cols + col]);
    }


    size_t const nr_rows;
    size_t const nr_cols;
    size_t const nr_cells;
    std::vector<std::int32_t> values;
    Datatype const memory_datatype;
    std::unique_ptr<Dataset> dataset;

};


BOOST_FIXTURE_TEST_CASE(read_all, Read2DDatasetFixture)
{
    std::vector<std::int32_t> values_read(nr_cells, 555);

    dataset->read(memory_datatype, values_read.data());

    BOOST_CHECK_EQUAL_COLLECTIONS(
        values_read.begin(), values_read.end(),
        values.begin(), values.end());
}


BOOST_FIXTURE_TEST_CASE(read_rows, Read2DDatasetFixture)
{
    for(size_t r = 0; r < nr_rows; ++r) {
        test_read_row(r);
    }
}


BOOST_FIXTURE_TEST_CASE(read_cols, Read2DDatasetFixture)
{
    for(size_t c = 0; c < nr_cols; ++c) {
        test_read_col(c);
    }
}


BOOST_FIXTURE_TEST_CASE(read_cell, Read2DDatasetFixture)
{
    for(size_t r = 0; r < nr_rows; ++r) {
        for(size_t c = 0; c < nr_cols; ++c) {
            test_read_cell(r, c);
        }
    }
}
