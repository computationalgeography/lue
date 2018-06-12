#define BOOST_TEST_MODULE lue time asynchronous_time_point
#include <boost/test/unit_test.hpp>
#include "lue/time/asynchronous_time_point.hpp"
#include "lue/test.hpp"
#include "lue/hdf5/file.hpp"
#include <numeric>


BOOST_AUTO_TEST_CASE(create_collection)
{
    using namespace lue;

    std::string const filename = "create_collection.h5";
    test::DatasetFixture fixture{filename};

    auto file = hdf5::create_file(filename);

    Clock clock(time::Unit::day, 1);

    auto collection = create_asynchronous_time_point(file, clock);

    BOOST_CHECK_EQUAL(collection.nr_items(), 0);

    hdf5::Shape const value_shape{1};
    BOOST_CHECK(collection.value_shape() == value_shape);

    hdf5::Datatype datatype{
        hdf5::NativeDatatypeTraits<time::DurationCount>::type_id()};
    BOOST_CHECK(collection.memory_datatype() == datatype);

    hsize_t const nr_items = 3;
    std::vector<hsize_t> const nr_time_domain_items = {
        2,
        0,
        4
    };
    collection.reserve(nr_items, nr_time_domain_items.data());

    BOOST_CHECK_EQUAL(collection.nr_items(), nr_items);

    for(hsize_t i = 0; i < nr_items; ++i) {
        BOOST_CHECK_EQUAL(
            collection.nr_time_domain_items(i),
            nr_time_domain_items[i]);
    }

    // Write and read values for each item
    std::vector<std::vector<time::DurationCount>> values(nr_items);
    for(hsize_t i = 0; i < nr_items; ++i) {
        auto const nr_time_domain_items_ = nr_time_domain_items[i];
        values[i].resize(nr_time_domain_items_);
        std::iota(values[i].begin(), values[i].end(), i);
        collection.write(i, values[i].data());
    }

    std::vector<std::vector<time::DurationCount>> values_read(nr_items);
    for(hsize_t i = 0; i < nr_items; ++i) {
        auto const nr_time_domain_items_ = nr_time_domain_items[i];

        values_read[i].resize(nr_time_domain_items_);
        collection.read(i, values_read[i].data());

        BOOST_CHECK_EQUAL_COLLECTIONS(
            values_read[i].begin(), values_read[i].end(),
            values[i].begin(), values[i].end());
    }

    // Write and read value for item with idx 2
    {
        hsize_t const item_idx = 2;
        auto const nr_time_domain_items_ = nr_time_domain_items[item_idx];

        std::vector<time::DurationCount> new_value(nr_time_domain_items_);
        std::iota(new_value.begin(), new_value.end(), 99);
        collection.write(item_idx, new_value.data());

        std::vector<time::DurationCount> value_read(nr_time_domain_items_);
        collection.read(item_idx, value_read.data());

        BOOST_CHECK_EQUAL_COLLECTIONS(
            value_read.begin(), value_read.end(),
            new_value.begin(), new_value.end());
    }

    // Write and read value for item with idx 0 and time domain item 1
    {
        hsize_t const item_idx = 0;
        hsize_t const time_domain_item_idx = 1;

        std::vector<time::DurationCount> new_value(1);
        std::iota(new_value.begin(), new_value.end(), 88);
        collection.write(item_idx, time_domain_item_idx, new_value.data());

        std::vector<time::DurationCount> value_read(1);
        collection.read(item_idx, time_domain_item_idx, value_read.data());

        BOOST_CHECK_EQUAL_COLLECTIONS(
            value_read.begin(), value_read.end(),
            new_value.begin(), new_value.end());
    }
}
