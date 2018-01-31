#define BOOST_TEST_MODULE lue item constant_collection constant_shape different_shape constant
#include <boost/test/unit_test.hpp>
#include "lue/item/constant_collection/constant_shape/different_shape/constant.hpp"
#include "lue/test.hpp"
#include "lue/hdf5/file.hpp"
#include <numeric>


BOOST_AUTO_TEST_CASE(create_collection)
{
    std::string const filename = "create_collection.h5";
    lue::test::DatasetFixture fixture{filename};

    auto file = lue::hdf5::create_file(filename);

    std::string const value_name = "my_value";
    lue::hdf5::Datatype datatype{
        lue::hdf5::NativeDatatypeTraits<int32_t>::type_id()};
    int const rank = 2;

    auto collection =
        lue::constant_collection::constant_shape::different_shape::create_constant(
            file, value_name, datatype, rank);

    BOOST_CHECK_EQUAL(collection.nr_items(), 0);
    BOOST_CHECK_EQUAL(collection.rank(), rank);
    BOOST_CHECK(collection.memory_datatype() == datatype);
    BOOST_CHECK(
        collection.file_datatype() == lue::hdf5::file_datatype(datatype));


    hsize_t const nr_items = 5;
    std::vector<hsize_t> const value_shapes = {
        11, 12,
        21, 22,
        31, 32,
        41, 42,
        51, 52,
    };
    collection.reserve(nr_items, value_shapes.data());

    BOOST_CHECK_EQUAL(collection.nr_items(), nr_items);

    // Write and read values for each item
    std::vector<std::vector<int32_t>> values(nr_items);
    for(hsize_t i = 0; i < nr_items; ++i) {
        lue::hdf5::Shape value_shape{
            value_shapes[i * rank + 0], value_shapes[i * rank + 1]};
        values[i].resize(value_shape[0] * value_shape[1]);
        std::iota(values[i].begin(), values[i].end(), i);
        collection.write(i, values[i].data());
    }

    std::vector<std::vector<int32_t>> values_read(nr_items);
    for(hsize_t i = 0; i < nr_items; ++i) {
        lue::hdf5::Shape value_shape{
            value_shapes[i * rank + 0], value_shapes[i * rank + 1]};
        auto value_shape_read = collection.value_shape(i);

        BOOST_CHECK(value_shape_read == value_shape);

        values_read[i].resize(value_shape[0] * value_shape[1]);
        collection.read(i, values_read[i].data());

        BOOST_CHECK_EQUAL_COLLECTIONS(
            values_read[i].begin(), values_read[i].end(),
            values[i].begin(), values[i].end());
    }

    // Write and read value for item with idx 2
    {
        hsize_t item_idx = 2;
        lue::hdf5::Shape value_shape{
            value_shapes[item_idx * rank + 0],
            value_shapes[item_idx * rank + 1]};
        std::vector<int32_t> new_value(value_shape[0] * value_shape[1]);
        std::iota(new_value.begin(), new_value.end(), 99);
        collection.write(item_idx, new_value.data());

        std::vector<int32_t> value_read(value_shape[0] * value_shape[1]);
        collection.read(item_idx, value_read.data());

        BOOST_CHECK_EQUAL_COLLECTIONS(
            value_read.begin(), value_read.end(),
            new_value.begin(), new_value.end());
    }
}
