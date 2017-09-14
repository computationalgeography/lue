#define BOOST_TEST_MODULE lue hl raster
#include <boost/test/unit_test.hpp>
#include "lue/hl/raster.hpp"


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

BOOST_AUTO_TEST_CASE(create_raster)
{
    std::string const dataset_name = "create_raster.lue";
    Fixture f(dataset_name);

    std::string const phenomenon_name = "my_phenomenon";
    std::string const property_set_name = "my_property_set";
    std::string const crs = "blah";
    double const west = 5.5;
    double const south = 4.5;
    double const north = 10.5;
    double const east = 11.5;
    hsize_t const nr_rows = 4;
    hsize_t const nr_cols = 3;


    // Create new dataset and add raster
    {
        auto dataset = lue::create_dataset(dataset_name);

        lue::hl::Raster::Domain domain(crs, west, south, north, east);
        lue::hl::Raster::Discretization discretization(nr_rows, nr_cols);

        // Add a raster
        auto raster = lue::hl::create_raster(
            dataset, phenomenon_name, property_set_name, domain,
            discretization);
    }


    // Verify the results
    BOOST_CHECK(lue::dataset_exists(dataset_name));

    // TODO
}

}  // Anonymous namespace
