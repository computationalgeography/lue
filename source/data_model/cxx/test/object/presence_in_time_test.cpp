#define BOOST_TEST_MODULE lue object presence_in_time
#include "lue/object/property/property_set.hpp"
#include "lue/test.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_FIXTURE_TEST_CASE(omnipresent_in_time, lue::data_model::test::DatasetFixture)
{
    std::string const phenomenon_name = "my_phenomenon";
    // std::string const property_set_name = "my_property_set";
    lue::data_model::Count const nr_objects = 5;

    std::vector<lue::data_model::ID> ids(nr_objects);
    lue::data_model::test::generate_random_ids(ids);

    // Create and write
    {
        auto& phenomenon = dataset().add_phenomenon(phenomenon_name);

        phenomenon.object_id().expand(nr_objects);
        phenomenon.object_id().write(ids.data());
    }

    // Open and read
    {
        auto const& phenomenon = dataset().phenomena()[phenomenon_name];

        BOOST_REQUIRE_EQUAL(phenomenon.object_id().nr_objects(), nr_objects);

        std::vector<lue::data_model::ID> ids_read(nr_objects);

        phenomenon.object_id().read(ids_read.data());
        BOOST_CHECK_EQUAL_COLLECTIONS(ids_read.begin(), ids_read.end(), ids.begin(), ids.end());
    }
}


BOOST_FIXTURE_TEST_CASE(time_points, lue::data_model::test::DatasetFixture)
{
    // std::string const phenomenon_name = "my_phenomenon";
    // std::string const property_set_name = "my_property_set";
    lue::data_model::Count const nr_locations_in_time = 25;

    // Per location in time the number of objects in the active set
    std::vector<lue::data_model::Count> nr_active_objects(nr_locations_in_time);
    lue::data_model::test::generate_random_counts(nr_active_objects, 5, 15);

    // Per location in time the IDs of the objects in the active set
    std::vector<std::vector<lue::data_model::ID>> active_ids(nr_locations_in_time);
    lue::data_model::test::select_random_ids(active_ids, nr_active_objects);

    // Create and write
    {}

    // Open and read
    {
    }
}


BOOST_FIXTURE_TEST_CASE(time_boxes, lue::data_model::test::DatasetFixture)
{
    // Create and write
    {}

    // Open and read
    {
    }
}


BOOST_FIXTURE_TEST_CASE(time_grids, lue::data_model::test::DatasetFixture)
{
    // Create and write
    {}

    // Open and read
    {
    }
}
