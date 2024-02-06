#define BOOST_TEST_MODULE lue gdal dataset_test
#include "lue/gdal/dataset.hpp"
#include "lue/gdal/driver.hpp"
#include <boost/test/unit_test.hpp>
#include <filesystem>


BOOST_AUTO_TEST_CASE(try_open_dataset)
{
    namespace lgd = lue::gdal;

    lgd::register_gdal_drivers();

    std::string const driver_name{"GTiff"};
    std::string const dataset_name{"try_open_dataset.tif"};
    lgd::Shape const raster_shape{60, 40};
    lgd::Count const nr_bands{1};
    GDALDataType const data_type{GDALDataType::GDT_Int32};

    if (std::filesystem::exists(dataset_name))
    {
        lgd::delete_dataset(*lgd::driver(driver_name), dataset_name);
    }

    // File does not exist
    BOOST_CHECK(!lgd::try_open_dataset(dataset_name, GDALAccess::GA_ReadOnly));
    BOOST_CHECK(!lgd::try_open_dataset(dataset_name, GDALAccess::GA_Update));

    lgd::create_dataset(driver_name, dataset_name, raster_shape, nr_bands, data_type);

    // File exists but is not readable and not writable
    std::filesystem::permissions(dataset_name, std::filesystem::perms::none);
    BOOST_CHECK(!lgd::try_open_dataset(dataset_name, GDALAccess::GA_ReadOnly));
    BOOST_CHECK(!lgd::try_open_dataset(dataset_name, GDALAccess::GA_Update));

    // File exists but is write-only: not readable
    std::filesystem::permissions(dataset_name, std::filesystem::perms::owner_write);
    BOOST_CHECK(!lgd::try_open_dataset(dataset_name, GDALAccess::GA_ReadOnly));
    BOOST_CHECK(!lgd::try_open_dataset(dataset_name, GDALAccess::GA_Update));

    // File exists and is read-only: not writable
    std::filesystem::permissions(dataset_name, std::filesystem::perms::owner_read);
    BOOST_CHECK(lgd::try_open_dataset(dataset_name, GDALAccess::GA_ReadOnly));
    BOOST_CHECK(!lgd::try_open_dataset(dataset_name, GDALAccess::GA_Update));

    // File exists and is readable and writable
    std::filesystem::permissions(
        dataset_name, std::filesystem::perms::owner_read | std::filesystem::perms::owner_write);
    BOOST_CHECK(lgd::try_open_dataset(dataset_name, GDALAccess::GA_ReadOnly));
    BOOST_CHECK(lgd::try_open_dataset(dataset_name, GDALAccess::GA_Update));
}


BOOST_AUTO_TEST_CASE(open_dataset)
{
    // TODO

    // namespace lgd = lue::gdal;

    // lgd::register_gdal_drivers();

    // std::string const driver_name{"GTiff"};
    // std::string const dataset_name{"open_dataset.tif"};
    // lgd::Shape const raster_shape{60, 40};
    // lgd::Count const nr_bands{1};
    // GDALDataType const data_type{GDALDataType::GDT_Int32};
}
