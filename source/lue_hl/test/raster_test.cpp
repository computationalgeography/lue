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
        // // Guarantees:
        // // - Dataset does not exist after teardown
        // remove_dataset();

        // BOOST_CHECK(!lue::dataset_exists(_dataset_name));
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
    hsize_t const nr_cells = nr_rows * nr_cols;

    std::string const band_name = "my_band";
    lue::hdf5::Datatype const memory_datatype = H5T_NATIVE_DOUBLE;
    std::array<double, nr_cells> values = {{
         0,  1,  2,
        10, 11, 12,
        20, 21, 22,
        30, 31, 32,
    }};


    // Create new dataset, add raster, add band, write values
    {
        auto dataset = lue::create_dataset(dataset_name);

        lue::hl::Raster::Domain domain(crs, west, south, east, north);
        lue::hl::Raster::Discretization discretization(nr_rows, nr_cols);

        // Add a raster
        auto raster = lue::hl::create_raster(
            dataset, phenomenon_name, property_set_name, domain,
            discretization);

        // Add a band
        auto band = raster.add_band(band_name, memory_datatype);

        // Write values
        band.write(values.data());
    }


    lue::hdf5::Datatype const file_datatype = H5T_IEEE_F64LE;

    // Open dataset, raster, band, read values
    {
        auto dataset = lue::Dataset(dataset_name);
        auto raster = lue::hl::Raster(
            dataset, phenomenon_name, property_set_name);

        // Test discretization info
        auto const& discretization = raster.discretization();

        BOOST_REQUIRE_EQUAL(discretization.nr_rows(), nr_rows);
        BOOST_REQUIRE_EQUAL(discretization.nr_cols(), nr_cols);

        // Test domain info
        auto const& domain = raster.domain();

        // TODO BOOST_CHECK_EQUAL(domain.crs(), crs);

        BOOST_CHECK_EQUAL(domain.west(), west);
        BOOST_CHECK_EQUAL(domain.south(), south);
        BOOST_CHECK_EQUAL(domain.east(), east);
        BOOST_CHECK_EQUAL(domain.north(), north);


        auto band = raster.band(band_name);

        // Test datatype
        auto const file_datatype_read = band.file_datatype();
        BOOST_REQUIRE(file_datatype_read == file_datatype);

        auto const memory_datatype_read = band.memory_datatype();
        BOOST_REQUIRE(memory_datatype_read == memory_datatype);

        // Read values into new buffer, verify result
        std::array<double, nr_cells> values_read;
        band.read(values_read.data());

        BOOST_CHECK_EQUAL_COLLECTIONS(
            values_read.begin(), values_read.end(),
            values.begin(), values.end());
    }
}


BOOST_AUTO_TEST_CASE(write_hyperslab)
{
    // Create raster, write values to hyperslab
    std::string const dataset_name = "write_hyperslab.lue";
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
    hsize_t const nr_cells = nr_rows * nr_cols;

    std::string const band_name = "my_band";
    lue::hdf5::Datatype const memory_datatype = H5T_NATIVE_INT32;
    std::array<std::int32_t, nr_cells> values = {{
         0,  1,  2,
        10, 11, 12,
        20, 21, 22,
        30, 31, 32,
    }};


    // Create new dataset, add raster, add band, write values
    {
        auto dataset = lue::create_dataset(dataset_name);

        lue::hl::Raster::Domain domain(crs, west, south, east, north);
        lue::hl::Raster::Discretization discretization(nr_rows, nr_cols);

        // Add a raster
        auto raster = lue::hl::create_raster(
            dataset, phenomenon_name, property_set_name, domain,
            discretization);

        // Add a band
        auto band = raster.add_band(band_name, memory_datatype);


        // Memory: linear array
        auto const memory_dataspace =
            lue::hdf5::create_dataspace(lue::hdf5::Shape{nr_cells});

        // File: 2D array
        lue::hdf5::Hyperslab hyperslab(
            lue::hdf5::Offset{0, 0},
            lue::hdf5::Stride{1, 1},
            lue::hdf5::Count{nr_rows, nr_cols}
        );

        // Write values
        band.write(memory_dataspace, hyperslab, values.data());
    }


    lue::hdf5::Datatype const file_datatype = H5T_STD_I32LE;

    // Open dataset, raster, band, read values
    {
        auto dataset = lue::Dataset(dataset_name);
        auto raster = lue::hl::Raster(
            dataset, phenomenon_name, property_set_name);

        // Test discretization info
        auto const discretization = raster.discretization();

        BOOST_REQUIRE_EQUAL(discretization.nr_rows(), nr_rows);
        BOOST_REQUIRE_EQUAL(discretization.nr_cols(), nr_cols);

        // Test domain info
        auto const& domain = raster.domain();

        // TODO BOOST_CHECK_EQUAL(domain.crs(), crs);

        BOOST_CHECK_EQUAL(domain.west(), west);
        BOOST_CHECK_EQUAL(domain.south(), south);
        BOOST_CHECK_EQUAL(domain.east(), east);
        BOOST_CHECK_EQUAL(domain.north(), north);


        auto band = raster.band(band_name);

        // Test datatype
        auto const file_datatype_read = band.file_datatype();
        BOOST_REQUIRE(file_datatype_read == file_datatype);

        auto const memory_datatype_read = band.memory_datatype();
        BOOST_REQUIRE(memory_datatype_read == memory_datatype);

        // Read values into new buffer, verify result
        std::array<std::int32_t, nr_cells> values_read;
        band.read(values_read.data());

        BOOST_CHECK_EQUAL_COLLECTIONS(
            values_read.begin(), values_read.end(),
            values.begin(), values.end());
    }
}

}  // Anonymous namespace
