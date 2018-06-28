#define BOOST_TEST_MODULE lue object presence_in_time
#include <boost/test/unit_test.hpp>
#include "lue/object/property/property_set.hpp"
#include "lue/test.hpp"


BOOST_FIXTURE_TEST_CASE(omnipresent_in_time, lue::test::DatasetFixture)
{
    std::string const phenomenon_name = "my_phenomenon";
    std::string const property_set_name = "my_property_set";
    lue::Count const nr_objects = 5;

    std::vector<lue::ID> ids(nr_objects);
    lue::test::generate_random_ids(ids);

    // Create and write
    {
        auto& phenomenon = dataset().add_phenomenon(phenomenon_name);
        auto& property_set = phenomenon.add_property_set(property_set_name);
        auto& object_tracker = property_set.object_tracker();

        object_tracker.id().reserve(nr_objects);
        object_tracker.id().write(ids.data());
    }

    // Open and read
    {
        auto const& phenomenon = dataset().phenomena()[phenomenon_name];
        auto const& property_set =
            phenomenon.property_sets()[property_set_name];
        auto const& object_tracker = property_set.object_tracker();

        BOOST_REQUIRE_EQUAL(object_tracker.id().nr_objects(), nr_objects);

        std::vector<lue::ID> ids_read(nr_objects);

        object_tracker.id().read(ids_read.data());
        BOOST_CHECK_EQUAL_COLLECTIONS(
            ids_read.begin(), ids_read.end(),
            ids.begin(), ids.end());
    }
}


BOOST_FIXTURE_TEST_CASE(time_points, lue::test::DatasetFixture)
{
    std::string const phenomenon_name = "my_phenomenon";
    std::string const property_set_name = "my_property_set";
    lue::Count const nr_locations_in_time = 25;

    // Per location in time the number of objects in the active set
    std::vector<lue::Count> nr_active_objects(nr_locations_in_time);
    lue::test::generate_random_counts(nr_active_objects, 5, 15);

    // Per location in time the IDs of the objects in the active set
    std::vector<std::vector<lue::ID>> active_ids(nr_locations_in_time);
    lue::test::select_random_ids(active_ids, nr_active_objects);

    // Create and write
    {
    }

    // Open and read
    {
    }
}


BOOST_FIXTURE_TEST_CASE(time_boxes, lue::test::DatasetFixture)
{
    // Create and write
    {
    }

    // Open and read
    {
    }
}


BOOST_FIXTURE_TEST_CASE(time_grids, lue::test::DatasetFixture)
{
    // Create and write
    {
    }

    // Open and read
    {
    }
}
