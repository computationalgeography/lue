#define BOOST_TEST_MODULE lue framework io write
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/uniform.hpp"
#include "lue/framework/algorithm/value_policies/unique_id.hpp"
#include "lue/framework/io/read_into.hpp"
#include "lue/framework/io/write_into.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/data_model/hl/raster_view.hpp"
#include "lue/framework.hpp"
#include <hpx/config.hpp>


BOOST_AUTO_TEST_CASE(use_case_1)
{
    // Write a stack of floating point rasters and read them back
    // in. Compare the rasters read with the range of the rasters written.
    namespace ldm = lue::data_model;

    std::string const dataset_pathname{"use_case_1.lue"};
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"area"};
    std::string const layer_name{"elevation"};
    std::string const array_pathname{
        fmt::format("{}/{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name, layer_name)};

    ldm::Count const nr_time_steps{50};
    ldm::Count const nr_rows{60};
    ldm::Count const nr_cols{40};

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
            {0, 0, 400, 600});
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

    for (ldm::Count time_step = 0; time_step <= nr_time_steps; ++time_step)
    {
        Array elevation_written = lue::default_policies::uniform(
            grid_shape,
            partition_shape,
            lowest_value + static_cast<Element>(time_step),
            highest_value + static_cast<Element>(time_step));
        write(elevation_written, array_pathname, object_id, static_cast<lue::Index>(time_step)).get();

        Array elevation_read = lue::read<Element, rank>(
            array_pathname, partition_shape, object_id, static_cast<lue::Index>(time_step));

        lue::test::check_arrays_are_equal(elevation_read, elevation_written);
    }
}


BOOST_AUTO_TEST_CASE(use_case_2)
{
    // Write a raster with integers and read it back in. Compare raster
    // written with raster read.
    namespace ldm = lue::data_model;

    std::string const dataset_pathname{"use_case_2.lue"};
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"area"};
    std::string const layer_name{"elevation"};
    std::string const array_pathname{
        fmt::format("{}/{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name, layer_name)};

    ldm::Count const nr_rows{62};
    ldm::Count const nr_cols{42};

    using Element = lue::LargestIntegralElement;

    ldm::ID object_id{};

    {
        // Prepare a dataset for storing the rasters in
        lue::hdf5::Shape raster_shape{nr_rows, nr_cols};

        using DatasetPtr = std::shared_ptr<ldm::Dataset>;
        using RasterView = ldm::constant::RasterView<DatasetPtr>;

        DatasetPtr dataset_ptr = std::make_shared<ldm::Dataset>(ldm::create_dataset(dataset_pathname));

        RasterView view = ldm::constant::create_raster_view(
            dataset_ptr, phenomenon_name, property_set_name, raster_shape, {0, 0, 400, 600});
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

    auto condition = lue::create_partitioned_array<lue::BooleanElement>(
        grid_shape, partition_shape, lue::BooleanElement{1});
    Array elevation_written = lue::value_policies::unique_id<Element>(condition);

    write(elevation_written, array_pathname, object_id).get();

    Array elevation_read = lue::read<Element, rank>(array_pathname, partition_shape, object_id);

    lue::test::check_arrays_are_equal(elevation_read, elevation_written);
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
//     lue::write(array, pathname);
//     BOOST_CHECK(std::filesystem::exists(pathname));
// }
