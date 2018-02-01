#define BOOST_TEST_MODULE lue space constant_collection stationary box
#include <boost/test/unit_test.hpp>
#include "lue/space/constant_collection/stationary/box.hpp"
#include "lue/test.hpp"
#include "lue/hdf5/file.hpp"


BOOST_AUTO_TEST_CASE(create_collection)
{
    using namespace lue::constant_collection;

    std::string const filename = "create_collection.h5";
    lue::test::DatasetFixture fixture{filename};

    auto file = lue::hdf5::create_file(filename);

    lue::hdf5::Datatype datatype{
        lue::hdf5::NativeDatatypeTraits<int32_t>::type_id()};
    std::size_t const rank = 2;

    auto collection = stationary::create_box(file, datatype, rank);

    BOOST_CHECK_EQUAL(collection.nr_items(), 0);

    lue::hdf5::Shape const value_shape{2 * rank};
    BOOST_CHECK(collection.value_shape() == value_shape);

    BOOST_CHECK(collection.memory_datatype() == datatype);
    BOOST_CHECK(
        collection.file_datatype() == lue::hdf5::file_datatype(datatype));

    hsize_t const nr_items = 3;
    collection.reserve(nr_items);

    BOOST_CHECK_EQUAL(collection.nr_items(), nr_items);

    // Write and read values for all items
    std::vector<int32_t> values = {
        // First item, 2 * 2 values
        11, 12, 13, 14,

        // Second item, 2 * 2 values
        21, 22, 23, 24,

        31, 32, 33, 34
    };
    collection.write(values.data());

    std::vector<int32_t> values_read(nr_items * 2 * rank);
    collection.read(values_read.data());

    BOOST_CHECK_EQUAL_COLLECTIONS(
        values_read.begin(), values_read.end(),
        values.begin(), values.end());

    // Write and read value for item with idx 2
    std::vector<int32_t> new_value = {
        51, 52, 53, 54
    };
    collection.write(2, new_value.data());

    std::vector<int32_t> value_read(2 * rank, 9);
    collection.read(2, value_read.data());

    BOOST_CHECK_EQUAL_COLLECTIONS(
        value_read.begin(), value_read.end(),
        new_value.begin(), new_value.end());
}
