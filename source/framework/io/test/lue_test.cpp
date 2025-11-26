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


namespace {

    using DatasetPtr = std::shared_ptr<lue::data_model::Dataset>;
    using ConstantRasterView = lue::data_model::constant::RasterView<DatasetPtr>;
    using VariableRasterView = lue::data_model::variable::RasterView<DatasetPtr>;
    using SpaceBox = ConstantRasterView::SpaceBox;
    using ObjectID = lue::data_model::ID;

    template<typename Element>
    using Array = lue::PartitionedArray<Element, 2>;

    using Shape = lue::Shape<lue::Count, 2>;


    template<typename Element>
    auto layout_constant_raster(
        std::string const& array_pathname, lue::hdf5::Shape const& raster_shape, SpaceBox const& space_box)
        -> ObjectID
    {
        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
            lue::parse_array_pathname(array_pathname);

        // The view grabs the dataset which must not go out of scope before the view has gone out of scope
        DatasetPtr dataset_ptr =
            std::make_shared<lue::data_model::Dataset>(lue::data_model::create_dataset(dataset_pathname));
        ConstantRasterView view = lue::data_model::constant::create_raster_view(
            dataset_ptr, phenomenon_name, property_set_name, raster_shape, space_box);

        view.add_layer<Element>(property_name);

        return view.object_id();
    }


    template<typename Element>
    auto layout_variable_raster(
        std::string const& array_pathname,
        lue::data_model::Clock const& clock,
        lue::data_model::Count const nr_time_steps,
        lue::hdf5::Shape const& raster_shape,
        SpaceBox const& space_box) -> ObjectID
    {
        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
            lue::parse_array_pathname(array_pathname);

        // The view grabs the dataset which must not go out of scope before the view has gone out of scope
        DatasetPtr dataset_ptr =
            std::make_shared<lue::data_model::Dataset>(lue::data_model::create_dataset(dataset_pathname));
        VariableRasterView view = lue::data_model::variable::create_raster_view(
            dataset_ptr,
            phenomenon_name,
            property_set_name,
            clock,
            nr_time_steps,
            {0, nr_time_steps},
            raster_shape,
            space_box);

        view.add_layer<Element>(property_name);

        return view.object_id();
    }


    template<typename Element>
    auto write_constant_raster(
        std::string const& array_pathname,
        ObjectID const object_id,
        lue::hdf5::Shape const& raster_shape,
        Shape const& partition_shape) -> std::tuple<Array<Element>, hpx::future<void>>
    {
        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
            lue::parse_array_pathname(array_pathname);

        Shape const grid_shape{
            static_cast<lue::Count>(raster_shape[0]), static_cast<lue::Count>(raster_shape[1])};

        Array<Element> array_written =
            lue::value_policies::uniform<Element>(grid_shape, partition_shape, Element{0}, Element{10});
        hpx::future<void> write_finished = lue::to_lue(array_written, array_pathname, object_id);

        return {std::move(array_written), std::move(write_finished)};
    }


    template<typename Element>
    auto write_variable_rasters(
        std::string const& array_pathname,
        ObjectID const object_id,
        lue::data_model::Count const nr_time_steps,
        lue::hdf5::Shape const& raster_shape,
        Shape const& partition_shape) -> std::tuple<std::vector<Array<Element>>, hpx::future<void>>
    {
        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
            lue::parse_array_pathname(array_pathname);

        Shape const grid_shape{
            static_cast<lue::Count>(raster_shape[0]), static_cast<lue::Count>(raster_shape[1])};

        std::vector<Array<Element>> arrays_written(nr_time_steps);
        std::vector<hpx::future<void>> writes_finished(nr_time_steps);

        for (lue::data_model::Count time_step = 0; time_step < nr_time_steps; ++time_step)
        {
            arrays_written[time_step] =
                lue::value_policies::uniform<Element>(grid_shape, partition_shape, Element{0}, Element{10});
            lue::to_lue(
                arrays_written[time_step], array_pathname, object_id, static_cast<lue::Index>(time_step))
                .get();
            // TODO: Asynchronous doesn't work (hang)?
            // writes_finished[time_step] = lue::to_lue(
            //     arrays_written[time_step], array_pathname, object_id, static_cast<lue::Index>(time_step));
        }

        // hpx::future<void> write_finished = hpx::when_all(writes_finished.begin(), writes_finished.end());
        hpx::future<void> write_finished = hpx::make_ready_future();

        return {std::move(arrays_written), std::move(write_finished)};
    }

}  // Anonymous namespace


// TODO: Make this works. Hangs?
// BOOST_AUTO_TEST_CASE(multiple_read_constant_raster_single_file)
// {
//     namespace ldm = lue::data_model;
//
//     std::string const
//     dataset_pathname{"lue_framework_io_lue_multiple_read_constant_raster_single_file.lue"}; std::string
//     const phenomenon_name{"area"}; std::string const property_set_name{"area"}; std::string const
//     property_name{"elevation"}; std::string const array_pathname{
//         std::format("{}/{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name, property_name)};
//
//     lue::Count const nr_rows_partition{6};
//     lue::Count const nr_cols_partition{4};
//     Shape const partition_shape{nr_rows_partition, nr_cols_partition};
//
//     // ldm::Count const nr_rows{lue::value_policies::uniform<ldm::Count>(100, 1000).future().get()};
//     // ldm::Count const nr_cols{lue::value_policies::uniform<ldm::Count>(100, 1000).future().get()};
//     lue::Count const nr_rows{60};
//     lue::Count const nr_cols{40};
//     lue::hdf5::Shape const raster_shape{nr_rows, nr_cols};
//     double const west{0};
//     double const south{0};
//     double const east{400};
//     double const north{600};
//     SpaceBox const space_box{west, south, east, north};
//
//     using Element = lue::LargestIntegralElement;
//
//     ObjectID const object_id = layout_constant_raster<Element>(array_pathname, raster_shape, space_box);
//
//     auto [array_written, write_finished] =
//         write_constant_raster<Element>(array_pathname, object_id, raster_shape, partition_shape);
//
//     write_finished.get();
//
//     std::size_t const nr_reads{2};
//     std::vector<Array<Element>> arrays_read(nr_reads);
//
//     for (std::size_t idx = 0; idx < nr_reads; ++idx)
//     {
//         arrays_read[idx] = lue::from_lue<Element>(array_pathname, partition_shape, object_id);
//     }
//
//     for (std::size_t idx = 0; idx < nr_reads; ++idx)
//     {
//         lue::test::check_arrays_are_equal(arrays_read[idx], array_written);
//     }
// }


// TODO: Make this work. Currently only the last raster is added to the dataset...
// BOOST_AUTO_TEST_CASE(multiple_write_constant_raster_single_file)
// {
//     namespace ldm = lue::data_model;
//
//     std::string const
//     dataset_pathname{"lue_framework_io_lue_multiple_write_constant_raster_single_file.lue"}; std::string
//     const phenomenon_name{"area"}; std::string const property_set_name{"area"}; std::string const
//     property_name{"elevation"};
//     // std::string const array_pathname{
//     //     std::format("{}/{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name,
//     property_name)};
//
//     lue::Count const nr_rows_partition{6};
//     lue::Count const nr_cols_partition{4};
//     [[maybe_unused]] Shape const partition_shape{nr_rows_partition, nr_cols_partition};
//
//     // ldm::Count const nr_rows{lue::value_policies::uniform<ldm::Count>(100, 1000).future().get()};
//     // ldm::Count const nr_cols{lue::value_policies::uniform<ldm::Count>(100, 1000).future().get()};
//     lue::Count const nr_rows{60};
//     lue::Count const nr_cols{40};
//     lue::hdf5::Shape const raster_shape{nr_rows, nr_cols};
//     double const west{0};
//     double const south{0};
//     double const east{400};
//     double const north{600};
//     SpaceBox const space_box{west, south, east, north};
//
//     using Element = lue::LargestIntegralElement;
//
//     std::size_t const nr_writes{5};
//     [[maybe_unused]] ObjectID object_id{};
//
//     for (std::size_t idx = 0; idx < nr_writes; ++idx)
//     {
//         std::string const array_pathname{std::format(
//             "{}/{}/{}/{}-{}", dataset_pathname, phenomenon_name, property_set_name, property_name, idx)};
//         object_id = layout_constant_raster<Element>(array_pathname, raster_shape, space_box);
//     }
//
//     // std::vector<hpx::future<void>> arrays_written(nr_writes);
//     // Shape const grid_shape{
//     //     static_cast<lue::Count>(raster_shape[0]), static_cast<lue::Count>(raster_shape[1])};
//     // Array<Element> array_written =
//     //     lue::value_policies::uniform<Element>(grid_shape, partition_shape, Element{0}, Element{10});
//     //
//     // for (std::size_t idx = 0; idx < nr_writes; ++idx)
//     // {
//     //     std::string const array_pathname{std::format(
//     //         "{}/{}/{}/{}-{}", dataset_pathname, phenomenon_name, property_set_name, property_name,
//     idx)};
//     //     arrays_written[idx] = lue::to_lue<Element>(array_written, array_pathname, object_id);
//     // }
//     //
//     // hpx::wait_all(arrays_written.begin(), arrays_written.end());
//     //
//     // for (std::size_t idx = 0; idx < nr_writes; ++idx)
//     // {
//     //     std::string const array_pathname{std::format(
//     //         "{}/{}/{}/{}-{}", dataset_pathname, phenomenon_name, property_set_name, property_name,
//     idx)};
//     //     Array const array_read = lue::from_lue<Element>(array_pathname, partition_shape, object_id);
//     //
//     //     lue::test::check_arrays_are_equal(array_read, array_written);
//     // }
// }


// TODO:
// BOOST_AUTO_TEST_CASE(multiple_read_write_constant_raster_single_file)
// {
// }


BOOST_AUTO_TEST_CASE(multiple_read_variable_raster_single_file)
{
    namespace ldm = lue::data_model;

    std::string const dataset_pathname{"lue_framework_io_lue_multiple_read_variable_raster_single_file.lue"};
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"area"};
    std::string const property_name{"elevation"};
    std::string const array_pathname{
        std::format("{}/{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name, property_name)};

    lue::Count const nr_rows_partition{6};
    lue::Count const nr_cols_partition{4};
    [[maybe_unused]] Shape const partition_shape{nr_rows_partition, nr_cols_partition};

    ldm::Clock const clock{lue::data_model::time::Unit::day, 1};
    // TODO: Make this work for multiple time steps
    ldm::Count const nr_time_steps{1};
    // ldm::Count const nr_rows{lue::value_policies::uniform<ldm::Count>(100, 1000).future().get()};
    // ldm::Count const nr_cols{lue::value_policies::uniform<ldm::Count>(100, 1000).future().get()};
    ldm::Count const nr_rows{60};
    ldm::Count const nr_cols{40};
    lue::hdf5::Shape const raster_shape{nr_rows, nr_cols};
    double const west{0};
    double const south{0};
    double const east{400};
    double const north{600};
    SpaceBox const space_box{west, south, east, north};

    using Element = lue::LargestIntegralElement;

    [[maybe_unused]] ObjectID const object_id =
        layout_variable_raster<Element>(array_pathname, clock, nr_time_steps, raster_shape, space_box);

    auto [arrays_written, write_finished] = write_variable_rasters<Element>(
        array_pathname, object_id, nr_time_steps, raster_shape, partition_shape);

    write_finished.get();

    std::vector<Array<Element>> arrays_read(nr_time_steps);

    // Read rasters without blocking. Collective calls should be called in the right order.
    for (lue::Index time_step = 0; time_step < static_cast<lue::Count>(nr_time_steps); ++time_step)
    {
        arrays_read[time_step] =
            lue::from_lue<Element>(array_pathname, partition_shape, object_id, time_step);
    }

    // TODO: Hangs
    // for (lue::Index time_step = 0; time_step < static_cast<lue::Count>(nr_time_steps); ++time_step)
    // {
    //     lue::test::check_arrays_are_equal(arrays_read[time_step], arrays_written[time_step]);
    // }
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
