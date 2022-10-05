#define BOOST_TEST_MODULE lue framework io raster
#include <hpx/config.hpp>
#include "lue/framework/io/raster.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(round_trip_1)
{
    // int32_t, all valid
    using Element = std::int32_t;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{5};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_raster_round_trip_1.tif"};

    lue::write<Element>(array_written, name).wait();

    Array array_read{lue::read<Element>(name, partition_shape)};

    lue::test::check_arrays_are_equal(array_read, array_written);
}


BOOST_AUTO_TEST_CASE(round_trip_2)
{
    // int32_t, none valid
    using Element = std::int32_t;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{lue::policy::no_data_value<Element>};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_raster_round_trip_2.tif"};

    lue::write<Element>(array_written, name).wait();

    Array array_read{lue::read<Element>(name, partition_shape)};

    lue::test::check_arrays_are_equal(array_read, array_written);
}


BOOST_AUTO_TEST_CASE(round_trip_3)
{
    // float, all valid
    using Element = float;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{5};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_raster_round_trip_3.tif"};

    lue::write<Element>(array_written, name).wait();

    Array array_read{lue::read<Element>(name, partition_shape)};

    lue::test::check_arrays_are_equal(array_read, array_written);
}


BOOST_AUTO_TEST_CASE(round_trip_4)
{
    // float, none valid
    using Element = float;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{lue::policy::no_data_value<Element>};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_raster_round_trip_4.tif"};

    lue::write<Element>(array_written, name).wait();

    Array array_read{lue::read<Element>(name, partition_shape)};

    lue::test::check_arrays_are_equal(array_read, array_written);
}
