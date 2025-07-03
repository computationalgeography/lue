#define BOOST_TEST_MODULE lue framework io lue
#include "lue/framework/algorithm/value_policies/uniform.hpp"
#include "lue/framework/io/from_lue.hpp"
#include "lue/framework/io/to_lue.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/data_model/hl/raster_view.hpp"
#include "lue/framework.hpp"
#include <hpx/config.hpp>


BOOST_AUTO_TEST_CASE(variable_raster)
{
    // Write a stack of floating point rasters and read them back in. Compare the rasters read with the range
    // of the rasters written.
    namespace ldm = lue::data_model;

    std::string const dataset_pathname{"lue_framework_io_lue_variable_raster.lue"};
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"area"};
    std::string const layer_name{"elevation"};
    std::string const array_pathname{
        std::format("{}/{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name, layer_name)};

    ldm::Count const nr_time_steps{5};
    ldm::Count const nr_rows{60};
    ldm::Count const nr_cols{40};

    double const west{0};
    double const south{0};
    double const east{400};
    double const north{600};

    ldm::ID object_id{};

    using Element = lue::FloatingPointElement<0>;

    {
        // Prepare a dataset for storing the rasters in
        ldm::Clock const clock{ldm::time::Unit::day, 1};
        lue::hdf5::Shape raster_shape{nr_rows, nr_cols};

        using DatasetPtr = std::shared_ptr<ldm::Dataset>;
        using RasterView = ldm::variable::RasterView<DatasetPtr>;

        DatasetPtr dataset_ptr = std::make_shared<ldm::Dataset>(ldm::create_dataset(dataset_pathname));

        RasterView view = ldm::variable::create_raster_view(
            dataset_ptr,
            phenomenon_name,
            property_set_name,
            // Also store initial state: nr_time_steps + 1
            clock,
            nr_time_steps + 1,
            {0, nr_time_steps + 1},
            raster_shape,
            {west, south, east, north});
        object_id = view.object_id();

        view.add_layer<Element>(layer_name);
    }

    // Create partitioned array
    ldm::Rank const rank{2};
    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const grid_shape{nr_rows, nr_cols};

    lue::Count const nr_rows_partition{6};
    lue::Count const nr_cols_partition{4};
    Shape const partition_shape{nr_rows_partition, nr_cols_partition};

    // elevation_t will contain values from the closed interval:
    // [lowest_value + t, highest_value + t]
    Element const lowest_value{-5000};
    Element const highest_value{std::nextafter(Element{-1000}, std::numeric_limits<Element>::max())};

    std::vector<Array> arrays_written(nr_time_steps + 1);

    // Create arrays
    for (ldm::Count time_step = 0; time_step <= nr_time_steps; ++time_step)
    {
        arrays_written[time_step] = lue::value_policies::uniform(
            grid_shape,
            partition_shape,
            lowest_value + static_cast<Element>(time_step),
            highest_value + static_cast<Element>(time_step));
    }

    // Write arrays
    for (ldm::Count time_step = 0; time_step <= nr_time_steps; ++time_step)
    {
        // Write and wait until finished
        lue::to_lue(arrays_written[time_step], array_pathname, object_id, static_cast<lue::Index>(time_step))
            .get();
    }

    // Read arrays
    for (ldm::Count time_step = 0; time_step <= nr_time_steps; ++time_step)
    {
        Array array_read = lue::from_lue<Element>(
            array_pathname, partition_shape, object_id, static_cast<lue::Index>(time_step));

        lue::test::check_arrays_are_equal(array_read, arrays_written[time_step]);
    }
}


BOOST_AUTO_TEST_CASE(constant_raster)
{
    // Write a constant raster with integers and read it back in. Compare raster written with raster read.
    namespace ldm = lue::data_model;

    std::string const dataset_pathname{"lue_framework_io_lue_constant_raster.lue"};
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"area"};
    std::string const layer_name{"elevation"};
    std::string const array_pathname{
        std::format("{}/{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name, layer_name)};

    ldm::Count const nr_rows{60};
    ldm::Count const nr_cols{40};
    double const west{0};
    double const south{0};
    double const east{400};
    double const north{600};

    using Element = lue::LargestIntegralElement;

    ldm::ID object_id{};

    {
        // Prepare a dataset for storing the rasters in
        lue::hdf5::Shape raster_shape{nr_rows, nr_cols};

        using DatasetPtr = std::shared_ptr<ldm::Dataset>;
        using RasterView = ldm::constant::RasterView<DatasetPtr>;

        DatasetPtr dataset_ptr = std::make_shared<ldm::Dataset>(ldm::create_dataset(dataset_pathname));

        RasterView view = ldm::constant::create_raster_view(
            dataset_ptr, phenomenon_name, property_set_name, raster_shape, {west, south, east, north});
        object_id = view.object_id();

        view.add_layer<Element>(layer_name);
    }

    // Create partitioned array
    ldm::Rank const rank{2};
    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const grid_shape{nr_rows, nr_cols};
    lue::Count const nr_rows_partition{6};
    lue::Count const nr_cols_partition{4};
    Shape const partition_shape{nr_rows_partition, nr_cols_partition};

    Array array_written =
        lue::value_policies::uniform<Element>(grid_shape, partition_shape, Element{0}, Element{10});

    // Write and wait until finished
    lue::to_lue(array_written, array_pathname, object_id).get();

    Array array_read = lue::from_lue<Element>(array_pathname, partition_shape, object_id);

    lue::test::check_arrays_are_equal(array_read, array_written);
}


// BOOST_AUTO_TEST_CASE(write_array_int32_2)
// {
//     using Element = std::int32_t;
//     lue::Rank const rank{2};
//     using Array = lue::PartitionedArray<Element, rank>;
//
//     auto const shape{lue::Test<Array>::shape()};
//
//     Array array{shape};
//     array = lue::default_policies::uniform(array, 0, 10);
//
//     std::string const pathname{"land_use.tif"};
//     lue::to_lue(array, pathname);
//     BOOST_CHECK(std::filesystem::exists(pathname));
// }
