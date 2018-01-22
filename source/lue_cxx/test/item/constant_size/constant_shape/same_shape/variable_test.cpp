#define BOOST_TEST_MODULE lue item constant_size constant_shape same_shape variable
#include <boost/test/unit_test.hpp>
#include "lue/item/constant_size/constant_shape/same_shape/variable.hpp"
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

        auto collection =
            same_shape::create_variable(dataset, value_name, datatype, value_shape);

        BOOST_CHECK_EQUAL(collection.nr_items(), 0);
        BOOST_CHECK(collection.value_shape() == value_shape);
        BOOST_CHECK(collection.memory_datatype() == datatype);
        BOOST_CHECK(
            collection.file_datatype() == lue::hdf5::file_datatype(datatype));


        hsize_t const nr_time_domain_items = 2;
        hsize_t const nr_items = 3;
        collection.reserve(nr_time_domain_items, nr_items);

        BOOST_CHECK_EQUAL(collection.nr_time_domain_items(), nr_time_domain_items);
        BOOST_CHECK_EQUAL(collection.nr_items(), nr_items);

        // Write and read values for all items
        std::vector<int32_t> values = {
            // First time step, first item, 6 values
            111, 112,
            113, 114,
            115, 116,

            // First time step, second item, 6 values
            121, 122,
            123, 124,
            125, 126,

            131, 132,
            133, 134,
            135, 136,

            211, 212,
            213, 214,
            215, 216,

            221, 222,
            223, 224,
            225, 226,

            231, 232,
            233, 234,
            235, 236,
        };
        collection.write(values.data());

        std::vector<int32_t> values_read(nr_time_domain_items * nr_items * nr_cells);
        collection.read(values_read.data());

        BOOST_CHECK_EQUAL_COLLECTIONS(
            values_read.begin(), values_read.end(),
            values.begin(), values.end());

        // Write and read value for time domain item with idx 0 and item with idx 2
        std::vector<int32_t> new_value = {
            531, 532,
            533, 534,
            535, 536,
        };
        collection.write(0, 2, new_value.data());

        std::vector<int32_t> value_read(nr_cells, 9);
        collection.read(0, 2, value_read.data());

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
        //     "variable");
    }
}
