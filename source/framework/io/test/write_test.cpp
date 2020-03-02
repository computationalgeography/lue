#define BOOST_TEST_MODULE lue framework io write
#include <hpx/config.hpp>
#include "lue/data_model/hl/raster_view.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/io/write.hpp"
#include "lue/framework/algorithm/uniform.hpp"
// #include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
// #include <boost/filesystem.hpp>


BOOST_AUTO_TEST_CASE(use_case_1)
{
    namespace ldm = lue::data_model;
    namespace lh5 = lue::hdf5;

    // Create a dataset
    std::string const dataset_pathname{"use_case_1.lue"};
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"area"};

    ldm::Clock const clock{ldm::time::Unit::day, 1};
    ldm::Count nr_time_steps{50};
    ldm::Count nr_rows{60};
    ldm::Count nr_cols{40};
    lh5::Shape raster_shape{nr_rows, nr_cols};

    using DatasetPtr = std::shared_ptr<ldm::Dataset>;
    using RasterView = ldm::variable::RasterView<DatasetPtr>;
    using RasterLayer = RasterView::Layer;

    DatasetPtr dataset_ptr = std::make_shared<ldm::Dataset>(
        ldm::create_in_memory_dataset(dataset_pathname));

    // Add raster layer
    RasterView view =
        ldm::variable::create_raster_view(
            dataset_ptr,
            phenomenon_name, property_set_name,
            // Also store initial state
            clock, nr_time_steps + 1, {0, nr_time_steps + 1},
            raster_shape, {0, 0, 400, 600});

    using Element = double;

    RasterLayer layer{view.add_layer<Element>("elevation")};

    // Create partitioned array
    ldm::Rank const rank{2};
    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape grid_shape;
    {
        // FIXME Make this easier
        auto const grid_shape1{view.grid_shape()};
        std::copy(grid_shape1.begin(), grid_shape1.end(), grid_shape.begin());
    }

    ldm::Index time_step{0};
    lue::Count nr_rows_partition{6};
    lue::Count nr_cols_partition{4};
    Shape partition_shape{nr_rows_partition, nr_cols_partition};

    // elevation_t will contain values from the closed interval:
    // [lowest_value + t, highest_value + t]
    Element lowest_value{0};
    Element highest_value{10};

    if constexpr(std::is_floating_point_v<Element>) {
        highest_value = std::nextafter(
            highest_value, std::numeric_limits<Element>::max());
    }

    Array elevation{grid_shape, partition_shape};
    elevation = lue::uniform(
        elevation,
        Element{lowest_value + time_step},
        Element{highest_value + time_step});

    // Write initial state
    std::vector<hpx::future<void>> written(nr_time_steps + 1);
    written[time_step] = write(elevation, layer, time_step);

    // Iterate through time and write temporal state
    for(++time_step; time_step <= nr_time_steps; ++time_step) {

        elevation = lue::uniform(
            elevation,
            Element{lowest_value + time_step},
            Element{highest_value + time_step});

        written[time_step] = write(elevation, layer, time_step);
    }

    hpx::wait_all(written);

    // Test contents of resulting dataset --------------------------------------
    for(time_step = 0; time_step <= nr_time_steps; ++time_step) {

        // TODO Figure out how to implement read
        // elevation = read(elevation, layer, time_step);

        // BOOST_CHECK(
        //     lue::all(
        //         lue::greater_equal(elevation, lowest_value + time_step)).get());
        // BOOST_CHECK(
        //     lue::all(
        //         lue::less_equal(elevation, highest_value + time_step)).get());

    }
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
//     array = lue::uniform(array, 0, 10);
// 
//     std::string const pathname{"land_use.tif"};
//     lue::write(array, pathname);
//     BOOST_CHECK(boost::filesystem::exists(pathname));
// }
