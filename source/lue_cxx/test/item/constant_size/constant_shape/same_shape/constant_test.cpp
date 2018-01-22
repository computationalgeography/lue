#define BOOST_TEST_MODULE lue item constant_size constant_shape same_shape constant
#include <boost/test/unit_test.hpp>
#include "lue/item/constant_size/constant_shape/same_shape/constant.hpp"
#include "lue/tag.hpp"
#include "lue/test.hpp"
#include "lue/hdf5/file.hpp"


BOOST_AUTO_TEST_CASE(create_collection)
{
    using namespace lue::constant_size::constant_shape;

    std::string const filename = "create_collection.h5";
    lue::test::DatasetFixture fixture{filename};

    {
        auto dataset = lue::hdf5::create_file(filename);

        std::string const value_name = "my_value";
        lue::hdf5::Datatype datatype{
            lue::hdf5::NativeDatatypeTraits<int32_t>::type_id()};
        std::size_t const nr_rows = 3;
        std::size_t const nr_cols = 2;
        std::size_t const nr_cells = nr_rows * nr_cols;
        lue::hdf5::Shape value_shape{nr_rows, nr_cols};

        auto collection = same_shape::create_constant(
            dataset, value_name, datatype, value_shape);

        BOOST_CHECK_EQUAL(collection.nr_items(), 0);
        BOOST_CHECK(collection.value_shape() == value_shape);
        BOOST_CHECK(collection.memory_datatype() == datatype);
        BOOST_CHECK(
            collection.file_datatype() == lue::hdf5::file_datatype(datatype));


        hsize_t const nr_items = 3;
        collection.reserve(nr_items);

        BOOST_CHECK_EQUAL(collection.nr_items(), nr_items);

        // Write and read values for all items
        std::vector<int32_t> values = {
            // First item, 6 values
            11, 12,
            13, 14,
            15, 16,

            // Second item, 6 values
            21, 22,
            23, 24,
            25, 26,

            31, 32,
            33, 34,
            35, 36,
        };
        collection.write(values.data());

        std::vector<int32_t> values_read(nr_items * nr_cells);
        collection.read(values_read.data());

        BOOST_CHECK_EQUAL_COLLECTIONS(
            values_read.begin(), values_read.end(),
            values.begin(), values.end());

        // Write and read value for item with idx 2
        std::vector<int32_t> new_value = {
            51, 52,
            53, 54,
            55, 56,
        };
        collection.write(2, new_value.data());

        std::vector<int32_t> value_read(nr_cells, 9);
        collection.read(2, value_read.data());

        BOOST_CHECK_EQUAL_COLLECTIONS(
            value_read.begin(), value_read.end(),
            new_value.begin(), new_value.end());
    }

    {
        auto const dataset = lue::hdf5::File(filename);

        // BOOST_CHECK_EQUAL(
        //     dataset.attribute<std::string>(lue::collection_variability_tag),
        //     "constant");
        // BOOST_CHECK_EQUAL(
        //     dataset.attribute<std::string>(lue::shape_variability_tag),
        //     "constant");
        // BOOST_CHECK_EQUAL(
        //     dataset.attribute<std::string>(lue::shape_per_item_tag),
        //     "same");
        // BOOST_CHECK_EQUAL(
        //     dataset.attribute<std::string>(lue::value_variability_tag),
        //     "constant");
    }
}
