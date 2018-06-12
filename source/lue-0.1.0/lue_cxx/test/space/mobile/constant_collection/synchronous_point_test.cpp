#define BOOST_TEST_MODULE lue space mobile constant_collection synchronous_point
#include <boost/test/unit_test.hpp>
#include "lue/space/mobile/constant_collection/synchronous_point.hpp"
#include "lue/test.hpp"
#include "lue/hdf5/file.hpp"


BOOST_AUTO_TEST_CASE(create_collection)
{
    std::string const filename = "create_collection.h5";
    lue::test::DatasetFixture fixture{filename};

    auto file = lue::hdf5::create_file(filename);

    lue::hdf5::Datatype datatype{
        lue::hdf5::NativeDatatypeTraits<int32_t>::type_id()};
    std::size_t const rank = 2;

    auto collection =
        lue::mobile::constant_collection::create_synchronous_point(
            file, datatype, rank);

    BOOST_CHECK_EQUAL(collection.nr_items(), 0);

    lue::hdf5::Shape const value_shape{rank};
    BOOST_CHECK(collection.value_shape() == value_shape);

    BOOST_CHECK(collection.memory_datatype() == datatype);
    BOOST_CHECK(
        collection.file_datatype() == lue::hdf5::file_datatype(datatype));

    hsize_t const nr_items = 3;
    hsize_t const nr_time_domain_items = 2;
    collection.reserve(nr_items, nr_time_domain_items);

    BOOST_CHECK_EQUAL(collection.nr_items(), nr_items);
    BOOST_CHECK_EQUAL(collection.nr_time_domain_items(), nr_time_domain_items);

    // Write and read values for all items
    std::vector<int32_t> const values = {
        // First item, first time step, 2 values
        111, 112,

        // Second item, first time step, 2 values
        211, 212,

        311, 312,

        // Second time step
        121, 122,
        221, 222,
        321, 322,
    };
    collection.write(values.data());

    std::vector<int32_t> values_read(nr_items * nr_time_domain_items * rank);
    collection.read(values_read.data());

    BOOST_CHECK_EQUAL_COLLECTIONS(
        values_read.begin(), values_read.end(),
        values.begin(), values.end());

    // Write and read value for time domain item with idx 0 and item with idx 2
    std::vector<int32_t> new_value = {
        531, 532,
    };
    collection.write(2, 0, new_value.data());

    std::vector<int32_t> value_read(rank, 9);
    collection.read(2, 0, value_read.data());

    BOOST_CHECK_EQUAL_COLLECTIONS(
        value_read.begin(), value_read.end(),
        new_value.begin(), new_value.end());
}
