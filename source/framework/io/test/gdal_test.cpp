#define BOOST_TEST_MODULE lue framework io gdal
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/io/gdal.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"
#include "lue/gdal.hpp"
#include <hpx/config.hpp>


BOOST_AUTO_TEST_CASE(round_trip_1)
{
    // Signed int, all valid
    using Element = lue::SignedIntegralElement<0>;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{5};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_raster_round_trip_1.tif"};

    lue::to_gdal<Element>(array_written, name).wait();

    BOOST_CHECK(lue::gdal::try_open_dataset(name, GDALAccess::GA_ReadOnly));

    Array array_read{lue::from_gdal<Element>(name, partition_shape)};

    lue::test::check_arrays_are_equal(array_read, array_written);
}


BOOST_AUTO_TEST_CASE(round_trip_2)
{
    // Signed int, none valid
    using Element = lue::SignedIntegralElement<0>;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{lue::policy::no_data_value<Element>};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_raster_round_trip_2.tif"};

    lue::to_gdal<Element>(array_written, name).wait();

    BOOST_CHECK(lue::gdal::try_open_dataset(name, GDALAccess::GA_ReadOnly));

    Array array_read{lue::from_gdal<Element>(name, partition_shape)};

    lue::test::check_arrays_are_equal(array_read, array_written);
}


BOOST_AUTO_TEST_CASE(round_trip_3)
{
    // Float, all valid
    using Element = lue::FloatingPointElement<0>;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{5};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_raster_round_trip_3.tif"};

    lue::to_gdal<Element>(array_written, name).wait();

    BOOST_CHECK(lue::gdal::try_open_dataset(name, GDALAccess::GA_ReadOnly));

    Array array_read{lue::from_gdal<Element>(name, partition_shape)};

    lue::test::check_arrays_are_equal(array_read, array_written);
}


BOOST_AUTO_TEST_CASE(round_trip_4)
{
    // Float, none valid
    using Element = lue::FloatingPointElement<0>;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{lue::policy::no_data_value<Element>};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_raster_round_trip_4.tif"};

    lue::to_gdal<Element>(array_written, name).wait();

    BOOST_CHECK(lue::gdal::try_open_dataset(name, GDALAccess::GA_ReadOnly));

    Array array_read{lue::from_gdal<Element>(name, partition_shape)};

    lue::test::check_arrays_are_equal(array_read, array_written);
}
