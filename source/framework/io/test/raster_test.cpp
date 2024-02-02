#define BOOST_TEST_MODULE lue framework io raster
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/io/raster.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/gdal.hpp"
#include <hpx/config.hpp>


BOOST_AUTO_TEST_CASE(round_trip_1)
{
    lue::gdal::register_gdal_drivers();

    // int32_t, all valid
    using Element = std::int32_t;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{5};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_raster_round_trip_1.tif"};

    lue::write<Element>(array_written, name).wait();

    // TODO BOOST_CHECK(lue::gdal::try_open_dataset(name, GDALAccess::GA_ReadOnly));

    // TODO Array array_read{lue::read<Element>(name, partition_shape)};

    // TODO lue::test::check_arrays_are_equal(array_read, array_written);
}


BOOST_AUTO_TEST_CASE(round_trip_2)
{
    lue::gdal::register_gdal_drivers();

    // int32_t, none valid
    using Element = std::int32_t;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{lue::policy::no_data_value<Element>};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_raster_round_trip_2.tif"};

    lue::write<Element>(array_written, name).wait();

    // TODO BOOST_CHECK(lue::gdal::try_open_dataset(name, GDALAccess::GA_ReadOnly));

    // TODO Array array_read{lue::read<Element>(name, partition_shape)};

    // TODO lue::test::check_arrays_are_equal(array_read, array_written);
}


BOOST_AUTO_TEST_CASE(round_trip_3)
{
    lue::gdal::register_gdal_drivers();

    // float, all valid
    using Element = float;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{5};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_raster_round_trip_3.tif"};

    lue::write<Element>(array_written, name).wait();

    // TODO BOOST_CHECK(lue::gdal::try_open_dataset(name, GDALAccess::GA_ReadOnly));

    // TODO Array array_read{lue::read<Element>(name, partition_shape)};

    // TODO lue::test::check_arrays_are_equal(array_read, array_written);
}


BOOST_AUTO_TEST_CASE(round_trip_4)
{
    lue::gdal::register_gdal_drivers();

    // float, none valid
    using Element = float;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{lue::policy::no_data_value<Element>};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_raster_round_trip_4.tif"};

    lue::write<Element>(array_written, name).wait();

    // TODO BOOST_CHECK(lue::gdal::try_open_dataset(name, GDALAccess::GA_ReadOnly));

    // TODO Array array_read{lue::read<Element>(name, partition_shape)};

    // TODO lue::test::check_arrays_are_equal(array_read, array_written);
}
