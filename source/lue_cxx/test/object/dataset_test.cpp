#define BOOST_TEST_MODULE lue object dataset
#include <boost/test/unit_test.hpp>
#include "lue/object/dataset.hpp"
#include "lue/test.hpp"


BOOST_AUTO_TEST_CASE(create_new_dataset)
{
    std::string const dataset_name = "create_new_dataset.lue";
    lue::test::FileFixture f{dataset_name};

    auto const dataset = lue::create_dataset(dataset_name);

    BOOST_REQUIRE(lue::dataset_exists(dataset_name));
    BOOST_CHECK_EQUAL(dataset.id().name(), "");
    BOOST_CHECK_EQUAL(dataset.id().pathname(), "/");
    BOOST_CHECK_EQUAL(dataset.pathname(), dataset_name);

    auto const& universes = dataset.universes();

    BOOST_CHECK_EQUAL(universes.size(), 0);

    auto const& phenomena = dataset.phenomena();

    BOOST_CHECK_EQUAL(phenomena.size(), 0);
}


BOOST_AUTO_TEST_CASE(open_new_dataset)
{
    std::string const dataset_name = "open_new_dataset.lue";
    lue::test::FileFixture f{dataset_name};

    /* auto const dataset = */ lue::create_dataset(dataset_name);

    BOOST_REQUIRE(lue::dataset_exists(dataset_name));

    lue::Dataset dataset{dataset_name, H5F_ACC_RDONLY};

    BOOST_CHECK_EQUAL(dataset.id().name(), "");
    BOOST_CHECK_EQUAL(dataset.id().pathname(), "/");
    BOOST_CHECK_EQUAL(dataset.pathname(), dataset_name);

    auto const& universes = dataset.universes();

    BOOST_CHECK_EQUAL(universes.size(), 0);

    auto const& phenomena = dataset.phenomena();

    BOOST_CHECK_EQUAL(phenomena.size(), 0);
}
