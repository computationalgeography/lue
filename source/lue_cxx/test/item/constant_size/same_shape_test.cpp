#define BOOST_TEST_MODULE lue item constant_size same_shape
#include <boost/test/unit_test.hpp>
#include "lue/item/constant_size/same_shape.hpp"
#include "lue/test/dataset_fixture.hpp"
#include "lue/hdf5/file.hpp"


BOOST_AUTO_TEST_CASE(create_same_shape)
{
    // TODO Update test to use a 2D value instead of a 0D one.

    std::string const filename = "create_same_shape.h5";
    lue::test::DatasetFixture fixture{filename};

    auto const dataset = lue::hdf5::create_file(filename);

    std::string const value_name = "my_value";
    lue::hdf5::Datatype datatype{
        lue::hdf5::NativeDatatypeTraits<int32_t>::type_id()};

    auto value = lue::constant_size::create_same_shape(
        dataset, value_name, datatype);

    BOOST_CHECK_EQUAL(value.nr_items(), 0);
    BOOST_CHECK(value.value_shape() == lue::hdf5::Shape{});
    BOOST_CHECK(value.memory_datatype() == datatype);
    BOOST_CHECK(value.file_datatype() == lue::hdf5::file_datatype(datatype));


    hsize_t const nr_items = 5;
    value.reserve(nr_items);

    BOOST_CHECK_EQUAL(value.nr_items(), nr_items);

    // Write and read values for all items
    std::vector<int32_t> values = { 2, 4, 6, 8, 10 };
    value.write(values.data());

    std::vector<int32_t> values_read(nr_items);
    value.read(values_read.data());

    BOOST_CHECK_EQUAL_COLLECTIONS(
        values_read.begin(), values_read.end(),
        values.begin(), values.end());

    // Write and read value for item with idx 2
    int32_t new_value = 5;
    value.write(2, &new_value);

    int32_t value_read = 9;
    value.read(2, &value_read);

    BOOST_CHECK_EQUAL(value_read, new_value);
}
