#define BOOST_TEST_MODULE lue gdal dataset_test
#include "lue/gdal/dataset.hpp"
#include "lue/gdal/driver.hpp"
#include "lue/stream.hpp"
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
        std::filesystem::permissions(dataset_name, std::filesystem::perms::all);
        lgd::delete_dataset(*lgd::driver(driver_name), dataset_name);
    }

    // File does not exist
    BOOST_CHECK(!lgd::try_open_dataset(dataset_name, GDALAccess::GA_ReadOnly));
    BOOST_CHECK(!lgd::try_open_dataset(dataset_name, GDALAccess::GA_Update));

    lgd::create_dataset(driver_name, dataset_name, raster_shape, nr_bands, data_type);

    // File exists but is not readable and not writable
    std::filesystem::permissions(dataset_name, std::filesystem::perms::none);
#ifndef _WIN32  // TODO
    BOOST_CHECK(!lgd::try_open_dataset(dataset_name, GDALAccess::GA_ReadOnly));
#endif
    BOOST_CHECK(!lgd::try_open_dataset(dataset_name, GDALAccess::GA_Update));

    // File exists but is write-only: not readable
    std::filesystem::permissions(dataset_name, std::filesystem::perms::owner_write);
#ifndef _WIN32  // TODO
    BOOST_CHECK(!lgd::try_open_dataset(dataset_name, GDALAccess::GA_ReadOnly));
    BOOST_CHECK(!lgd::try_open_dataset(dataset_name, GDALAccess::GA_Update));
#endif

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
    namespace lgd = lue::gdal;

    lgd::register_gdal_drivers();

    std::string const driver_name{"GTiff"};
    std::string const dataset_name{"open_dataset.tif"};
    lgd::Shape const raster_shape{60, 40};
    lgd::Count const nr_bands{1};
    GDALDataType const data_type{GDALDataType::GDT_Int32};

    if (std::filesystem::exists(dataset_name))
    {
        std::filesystem::permissions(dataset_name, std::filesystem::perms::all);
        lgd::delete_dataset(*lgd::driver(driver_name), dataset_name);
    }

    // File does not exist
    BOOST_CHECK_THROW(lgd::open_dataset(dataset_name, GDALAccess::GA_ReadOnly), std::runtime_error);


    BOOST_CHECK_THROW(lgd::open_dataset(dataset_name, GDALAccess::GA_Update), std::runtime_error);

    lgd::create_dataset(driver_name, dataset_name, raster_shape, nr_bands, data_type);

    // File exists but is not readable and not writable
    std::filesystem::permissions(dataset_name, std::filesystem::perms::none);
#ifndef _WIN32  // TODO
    BOOST_CHECK_THROW(lgd::open_dataset(dataset_name, GDALAccess::GA_ReadOnly), std::runtime_error);
#endif
    BOOST_CHECK_THROW(lgd::open_dataset(dataset_name, GDALAccess::GA_Update), std::runtime_error);

    // File exists but is write-only: not readable
    std::filesystem::permissions(dataset_name, std::filesystem::perms::owner_write);
#ifndef _WIN32  // TODO
    BOOST_CHECK_THROW(lgd::open_dataset(dataset_name, GDALAccess::GA_ReadOnly), std::runtime_error);
    BOOST_CHECK_THROW(lgd::open_dataset(dataset_name, GDALAccess::GA_Update), std::runtime_error);
#endif

    // File exists and is read-only: not writable
    std::filesystem::permissions(dataset_name, std::filesystem::perms::owner_read);
    BOOST_CHECK_NO_THROW(lgd::open_dataset(dataset_name, GDALAccess::GA_ReadOnly));
    BOOST_CHECK_THROW(lgd::open_dataset(dataset_name, GDALAccess::GA_Update), std::runtime_error);

    // File exists and is readable and writable
    std::filesystem::permissions(
        dataset_name, std::filesystem::perms::owner_read | std::filesystem::perms::owner_write);
    BOOST_CHECK_NO_THROW(lgd::open_dataset(dataset_name, GDALAccess::GA_ReadOnly));
    BOOST_CHECK_NO_THROW(lgd::open_dataset(dataset_name, GDALAccess::GA_Update));
}


BOOST_AUTO_TEST_CASE(create_dataset)
{
    namespace lgd = lue::gdal;

    lgd::register_gdal_drivers();

    std::string const driver_name{"GTiff"};
    std::string const dataset_name{"create_dataset.tif"};
    lgd::Shape const raster_shape{60, 40};
    lgd::Count const nr_bands{2};
    GDALDataType const data_type{GDALDataType::GDT_Int32};

    if (std::filesystem::exists(dataset_name))
    {
        lgd::delete_dataset(*lgd::driver(driver_name), dataset_name);
    }

    BOOST_CHECK(!std::filesystem::exists(dataset_name));

    lgd::create_dataset(driver_name, dataset_name, raster_shape, nr_bands, data_type);

    BOOST_CHECK(std::filesystem::exists(dataset_name));

    auto const dataset_ptr = lgd::open_dataset(dataset_name, GDALAccess::GA_ReadOnly);

    BOOST_CHECK_EQUAL(lgd::shape(*dataset_ptr), raster_shape);
    BOOST_CHECK_EQUAL(lgd::nr_raster_bands(*dataset_ptr), nr_bands);
    BOOST_CHECK_EQUAL(lgd::data_type(*dataset_ptr), data_type);
}


BOOST_AUTO_TEST_CASE(create_copy)
{
    namespace lgd = lue::gdal;

    lgd::register_gdal_drivers();

    std::string const driver_name{"GTiff"};
    std::string const dataset_name{"create_copy.tif"};
    std::string const dataset_copy_name{"create_copy_copy.tif"};
    lgd::Shape const raster_shape{60, 40};
    lgd::Count const nr_bands{2};
    GDALDataType const data_type{GDALDataType::GDT_Int32};

    if (std::filesystem::exists(dataset_name))
    {
        lgd::delete_dataset(*lgd::driver(driver_name), dataset_name);
    }

    lgd::create_dataset(driver_name, dataset_name, raster_shape, nr_bands, data_type);

    auto dataset_ptr = lgd::open_dataset(dataset_name, GDALAccess::GA_ReadOnly);

    auto const dataset_copy_ptr = lgd::create_copy(dataset_copy_name, *dataset_ptr);

    BOOST_CHECK_EQUAL(lgd::shape(*dataset_copy_ptr), raster_shape);
    BOOST_CHECK_EQUAL(lgd::nr_raster_bands(*dataset_copy_ptr), nr_bands);
    BOOST_CHECK_EQUAL(lgd::data_type(*dataset_copy_ptr), data_type);
}
