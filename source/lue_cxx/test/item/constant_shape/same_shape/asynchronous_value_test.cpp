#define BOOST_TEST_MODULE lue item constant_shape same_shape asynchronous_value
#include <boost/test/unit_test.hpp>
#include "lue/item/constant_shape/same_shape/asynchronous_value.hpp"
#include "lue/test.hpp"
#include "lue/hdf5/file.hpp"
#include <numeric>
#include <iostream>


BOOST_AUTO_TEST_CASE(create_collection)
{
    std::string const filename = "create_collection.h5";
    lue::test::DatasetFixture fixture{filename};

    auto file = lue::hdf5::create_file(filename);

    std::string const value_name = "my_value";
    lue::hdf5::Datatype datatype{
        lue::hdf5::NativeDatatypeTraits<int32_t>::type_id()};
    std::size_t const nr_rows = 3;
    std::size_t const nr_cols = 2;
    // std::size_t const nr_cells = nr_rows * nr_cols;
    lue::hdf5::Shape const value_shape{nr_rows, nr_cols};

    auto collection =
        lue::constant_shape::same_shape::create_asynchronous_value(
            file, value_name, datatype, value_shape);

    BOOST_CHECK_EQUAL(collection.nr_items(), 0);
    BOOST_CHECK(collection.value_shape() == value_shape);
    BOOST_CHECK(collection.memory_datatype() == datatype);
    BOOST_CHECK(
        collection.file_datatype() == lue::hdf5::file_datatype(datatype));

    hsize_t const nr_items = 3;
    std::vector<hsize_t> const nr_time_domain_items = {
        2,
        0,
        4
    };
    // std::vector<hsize_t> value_shapes = {
    //     11, 12,
    //     21, 22,
    //     31, 32,
    // };
    collection.reserve(nr_items, nr_time_domain_items.data());

    BOOST_CHECK_EQUAL(collection.nr_items(), nr_items);

    for(hsize_t i = 0; i < nr_items; ++i) {
        BOOST_CHECK_EQUAL(
            collection.nr_time_domain_items(i),
            nr_time_domain_items[i]);
    }

    // Write and read values for each item
    std::vector<std::vector<int32_t>> values(nr_items);
    for(hsize_t i = 0; i < nr_items; ++i) {
        auto const nr_time_domain_items_ = nr_time_domain_items[i];
        values[i].resize(
            nr_time_domain_items_ * value_shape[0] * value_shape[1]);
        std::iota(values[i].begin(), values[i].end(), i);
        collection.write(i, values[i].data());
    }

    std::vector<std::vector<int32_t>> values_read(nr_items);
    for(hsize_t i = 0; i < nr_items; ++i) {
        auto const nr_time_domain_items_ = nr_time_domain_items[i];
        values_read[i].resize(
            nr_time_domain_items_ * value_shape[0] * value_shape[1]);
        collection.read(i, values_read[i].data());

        BOOST_CHECK_EQUAL_COLLECTIONS(
            values_read[i].begin(), values_read[i].end(),
            values[i].begin(), values[i].end());
    }

    // Write and read value for item with idx 2
    {
        hsize_t const item_idx = 2;
        auto const nr_time_domain_items_ = nr_time_domain_items[item_idx];
        std::vector<int32_t> new_value(
            nr_time_domain_items_ * value_shape[0] * value_shape[1]);
        std::iota(new_value.begin(), new_value.end(), 99);
        collection.write(item_idx, new_value.data());

        std::vector<int32_t> value_read(
            nr_time_domain_items_ * value_shape[0] * value_shape[1]);
        collection.read(item_idx, value_read.data());

        BOOST_CHECK_EQUAL_COLLECTIONS(
            value_read.begin(), value_read.end(),
            new_value.begin(), new_value.end());
    }

    // Write and read value for item with idx 0 and time domain item 1
    {
        hsize_t const item_idx = 0;
        hsize_t const time_domain_item_idx = 1;

        std::vector<int32_t> new_value(value_shape[0] * value_shape[1]);
        std::iota(new_value.begin(), new_value.end(), 88);
        collection.write(item_idx, time_domain_item_idx, new_value.data());

        std::vector<int32_t> value_read(
            value_shape[0] * value_shape[1]);
        collection.read(item_idx, time_domain_item_idx, value_read.data());

        BOOST_CHECK_EQUAL_COLLECTIONS(
            value_read.begin(), value_read.end(),
            new_value.begin(), new_value.end());
    }
}
