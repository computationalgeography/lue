#define BOOST_TEST_MODULE lue dataset
#include <boost/test/unit_test.hpp>
#include "lue/dataset.h"


namespace {

class Fixture
{

public:

    Fixture(
        std::string const& dataset_name)

        : _dataset_name(dataset_name)

    {
        // Guarantees:
        // - Dataset does not exist after setup
        remove_dataset();

        BOOST_REQUIRE(!lue::dataset_exists(_dataset_name));
    }


    ~Fixture()
    {
        // Guarantees:
        // - Dataset does not exist after teardown
        remove_dataset();

        BOOST_CHECK(!lue::dataset_exists(_dataset_name));
    }


private:

    std::string const _dataset_name;

    void remove_dataset()
    {
        if(lue::dataset_exists(_dataset_name)) {
            lue::remove_dataset(_dataset_name);
        }
    }

};

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(create_new_dataset)
{
    std::string const dataset_name = "create_new_dataset.lue";
    Fixture f(dataset_name);

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
    Fixture f(dataset_name);

    /* auto const dataset = */ lue::create_dataset(dataset_name);

    BOOST_REQUIRE(lue::dataset_exists(dataset_name));

    lue::Dataset dataset(dataset_name, H5F_ACC_RDONLY);

    BOOST_CHECK_EQUAL(dataset.id().name(), "");
    BOOST_CHECK_EQUAL(dataset.id().pathname(), "/");
    BOOST_CHECK_EQUAL(dataset.pathname(), dataset_name);

    auto const& universes = dataset.universes();

    BOOST_CHECK_EQUAL(universes.size(), 0);

    auto const& phenomena = dataset.phenomena();

    BOOST_CHECK_EQUAL(phenomena.size(), 0);
}
