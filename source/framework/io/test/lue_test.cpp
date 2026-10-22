#define BOOST_TEST_MODULE lue framework io lue
#include "lue/framework/algorithm/value_policies/uniform.hpp"
#include "lue/framework/io/from_lue.hpp"
#include "lue/framework/io/to_lue.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/data_model/hl/raster_view.hpp"
#include "lue/framework.hpp"
#include <hpx/config.hpp>
#include <boost/predef.h>


// TODO: Waiting for to_lue to finish fixes an error from occurring in ~5% of the cases:

// clang-format off
//
// 184: HDF5-DIAG: Error detected in HDF5 (1.10.10) thread 1:
// 184:   #000: ../../../src/H5F.c line 412 in H5Fopen(): unable to open file
// 184:     major: File accessibility
// 184:     minor: Unable to open file
// 184:   #001: ../../../src/H5Fint.c line 1698 in H5F_open(): file is already open for read-only
// 184:     major: File accessibility
// 184:     minor: Unable to open file

// HDF5-DIAG: Error detected in HDF5 (1.14.6):
//   #000: /usr/src/debug/hdf5/hdf5-hdf5_1.14.6/src/H5D.c line 1044 in H5Dread(): can't synchronously read data
//     major: Dataset
// HDF5-DIAG: Error detected in HDF5 (1.14.6):
//   #000: /usr/src/debug/hdf5/hdf5-hdf5_1.14.6/src/H5F.c line 827 in H5Fopen(): unable to synchronously open file
//     major: File accessibility
//     minor: Unable to open file
//   #001: /usr/src/debug/hdf5/hdf5-hdf5_1.14.6/src/H5F.c line 788 in H5F__open_api_common(): unable to open file
//     major: File accessibility
//     minor: Unable to open file
//     minor: Read failed
//   #007: /usr/src/debug/hdf5/hdf5-hdf5_1.14.6/src/H5D.c line 992 in H5D__read_api_common(): can't read data
//     major: Dataset
//     minor: Read failed
//   #008: /usr/src/debug/hdf5/hdf5-hdf5_1.14.6/src/H5VLcallback.c line 2083 in H5VL_dataset_read(): can't reset VOL wrapper info
//     major: Virtual Object Layer
//     minor: Can't reset object
//   #009: /usr/src/debug/hdf5/hdf5-hdf5_1.14.6/src/H5VLint.c line 2406 in H5VL_reset_vol_wrapper(): no VOL object wrap context?
//     major: Virtual Object Layer
//     minor: Bad value
//   #002: /usr/src/debug/hdf5/hdf5-hdf5_1.14.6/src/H5VLcallback.c line 3680 in H5VL_file_open(): open failed
//     major: Virtual Object Layer
//     minor: Can't open object
//   #003: /usr/src/debug/hdf5/hdf5-hdf5_1.14.6/src/H5VLcallback.c line 3514 in H5VL__file_open(): open failed
//     major: Virtual Object Layer
//     minor: Can't open object
//   #004: /usr/src/debug/hdf5/hdf5-hdf5_1.14.6/src/H5VLnative_file.c line 128 in H5VL__native_file_open(): unable to open file
//     major: File accessibility
//     minor: Unable to open file
//   #005: /usr/src/debug/hdf5/hdf5-hdf5_1.14.6/src/H5Fint.c line 1925 in H5F_open(): file is already open for read-only
//     major: File accessibility
//     minor: Unable to open file
//   #006: /usr/src/debug/hdf5/hdf5-hdf5_1.14.6/src/H5D.c line 1044 in H5Dread(): can't synchronously read data
//     major: Dataset
//     minor: Read failed
//   #007: /usr/src/debug/hdf5/hdf5-hdf5_1.14.6/src/H5D.c line 992 in H5D__read_api_common(): can't read data
//     major: Dataset
//     minor: Read failed
//   #008: /usr/src/debug/hdf5/hdf5-hdf5_1.14.6/src/H5VLcallback.c line 2083 in H5VL_dataset_read(): can't reset VOL wrapper info
//     major: Virtual Object Layer
//     minor: Can't reset object
//   #009: /usr/src/debug/hdf5/hdf5-hdf5_1.14.6/src/H5VLint.c line 2406 in H5VL_reset_vol_wrapper(): no VOL object wrap context?
//     major: Virtual Object Layer
//     minor: Bad value
//
// clang-format on
//
// The error suggests that to_lue can't open the dataset because from_lue is not ready reading from it.
//
// Waiting for to_lue to finish prevents the issue. Weird thing is that moving these synchronization points
// elsewhere, like into to_lue or from_lue does not prevent the issue.
//
// The good thing is that the issue only occurs when writing and reading from/to the same file. IRL, data
// different files will likely be used for reading vs writing: output_files = my_model(input_files)
//
// https://github.com/computationalgeography/lue/issues/945


namespace {

    using DatasetPtr = std::shared_ptr<lue::data_model::Dataset>;
    using ConstantRasterView = lue::data_model::constant::RasterView<DatasetPtr>;
    using VariableRasterView = lue::data_model::variable::RasterView<DatasetPtr>;
    using SpaceBox = ConstantRasterView::SpaceBox;
    using ObjectID = lue::data_model::ID;

    template<typename Element>
    using Array = lue::PartitionedArray<Element, 2>;

    using Shape = lue::Shape<lue::Count, 2>;


    auto layout_raster() -> std::tuple<Shape, Shape>
    {
        using NrElements = lue::LargestIntegralElement;

        lue::Count const nr_rows{
            static_cast<lue::Count>(lue::value_policies::uniform<NrElements>(100, 1000).future().get())};
        lue::Count const nr_cols{
            static_cast<lue::Count>(lue::value_policies::uniform<NrElements>(100, 1000).future().get())};
        lue::Count const nr_rows_partition{
            static_cast<lue::Count>(lue::value_policies::uniform<NrElements>(10, 100).future().get())};
        lue::Count const nr_cols_partition{
            static_cast<lue::Count>(lue::value_policies::uniform<NrElements>(10, 100).future().get())};

        Shape const array_shape{nr_rows, nr_cols};
        Shape const partition_shape{nr_rows_partition, nr_cols_partition};

        return {array_shape, partition_shape};
    }


    template<typename Element>
    auto layout_constant_raster(std::string const& array_pathname) -> std::tuple<ObjectID, Shape, Shape>
    {
        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
            lue::parse_array_pathname(array_pathname);

        auto const [array_shape, partition_shape] = layout_raster();
        auto const [nr_rows, nr_cols] = array_shape;

        double const cell_size{10};
        double const west{0};
        double const south{0};
        double const east{cell_size * static_cast<double>(nr_cols)};
        double const north{cell_size * static_cast<double>(nr_rows)};
        SpaceBox const space_box{west, south, east, north};

        // The view grabs the dataset which must not go out of scope before the view has gone out of scope
        DatasetPtr dataset_ptr =
            std::make_shared<lue::data_model::Dataset>(lue::data_model::create_dataset(dataset_pathname));

        ConstantRasterView view = lue::data_model::constant::create_raster_view(
            dataset_ptr,
            phenomenon_name,
            property_set_name,
            {static_cast<lue::data_model::Count>(array_shape[0]),
             static_cast<lue::data_model::Count>(array_shape[1])},
            space_box);

        view.add_layer<Element>(property_name);

        return {view.object_id(), array_shape, partition_shape};
    }


    template<typename Element>
    auto layout_constant_rasters(
        std::string const& property_set_pathname, Shape const& array_shape, lue::Count const nr_rasters)
        -> std::tuple<ObjectID, std::vector<std::string>>
    {
        auto const [dataset_pathname, phenomenon_name, property_set_name, _] =
            lue::parse_array_pathname(std::format("{}/{}", property_set_pathname, "whatever"));

        auto const [nr_rows, nr_cols] = array_shape;

        double const cell_size{10};
        double const west{0};
        double const south{0};
        double const east{cell_size * static_cast<double>(nr_cols)};
        double const north{cell_size * static_cast<double>(nr_rows)};
        SpaceBox const space_box{west, south, east, north};

        // The view grabs the dataset which must not go out of scope before the view has gone out of scope
        DatasetPtr dataset_ptr =
            std::make_shared<lue::data_model::Dataset>(lue::data_model::create_dataset(dataset_pathname));

        ConstantRasterView view = lue::data_model::constant::create_raster_view(
            dataset_ptr,
            phenomenon_name,
            property_set_name,
            {static_cast<lue::data_model::Count>(array_shape[0]),
             static_cast<lue::data_model::Count>(array_shape[1])},
            space_box);

        std::string const property_basename = "property";
        std::vector<std::string> property_names(nr_rasters);

        for (lue::Count raster_idx = 0; raster_idx < nr_rasters; ++raster_idx)
        {
            std::string const property_name = std::format("{}-{}", property_basename, raster_idx);
            view.add_layer<Element>(property_name);
            property_names[raster_idx] = property_name;
        }

        return {view.object_id(), property_names};
    }


    template<typename Element>
    auto layout_variable_raster(std::string const& array_pathname)
        -> std::tuple<ObjectID, lue::Count, Shape, Shape>
    {
        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
            lue::parse_array_pathname(array_pathname);

        using NrElements = lue::LargestIntegralElement;

        auto const [array_shape, partition_shape] = layout_raster();
        auto const [nr_rows, nr_cols] = array_shape;

        lue::Count const nr_time_steps{
            static_cast<lue::Count>(lue::value_policies::uniform<NrElements>(5, 20).future().get())};

        lue::data_model::Clock const clock{lue::data_model::time::Unit::day, 1};

        double const cell_size{10};
        double const west{0};
        double const south{0};
        double const east{cell_size * static_cast<double>(nr_cols)};
        double const north{cell_size * static_cast<double>(nr_rows)};
        SpaceBox const space_box{west, south, east, north};

        // The view grabs the dataset which must not go out of scope before the view has gone out of scope
        DatasetPtr dataset_ptr =
            std::make_shared<lue::data_model::Dataset>(lue::data_model::create_dataset(dataset_pathname));

        VariableRasterView view = lue::data_model::variable::create_raster_view(
            dataset_ptr,
            phenomenon_name,
            property_set_name,
            clock,
            static_cast<lue::data_model::Count>(nr_time_steps),
            {0, static_cast<lue::data_model::Count>(nr_time_steps)},
            {static_cast<lue::data_model::Count>(array_shape[0]),
             static_cast<lue::data_model::Count>(array_shape[1])},
            space_box);

        view.add_layer<Element>(property_name);

        return {view.object_id(), nr_time_steps, array_shape, partition_shape};
    }


    template<typename Element>
    auto layout_variable_rasters(
        std::string const& property_set_pathname,
        lue::Count const nr_time_steps,
        Shape const& array_shape,
        lue::Count const nr_rasters) -> std::tuple<ObjectID, std::vector<std::string>>
    {
        auto const [dataset_pathname, phenomenon_name, property_set_name, _] =
            lue::parse_array_pathname(std::format("{}/{}", property_set_pathname, "whatever"));

        auto const [nr_rows, nr_cols] = array_shape;

        lue::data_model::Clock const clock{lue::data_model::time::Unit::day, 1};

        double const cell_size{10};
        double const west{0};
        double const south{0};
        double const east{cell_size * static_cast<double>(nr_cols)};
        double const north{cell_size * static_cast<double>(nr_rows)};
        SpaceBox const space_box{west, south, east, north};

        // The view grabs the dataset which must not go out of scope before the view has gone out of scope
        DatasetPtr dataset_ptr =
            std::make_shared<lue::data_model::Dataset>(lue::data_model::create_dataset(dataset_pathname));

        VariableRasterView view = lue::data_model::variable::create_raster_view(
            dataset_ptr,
            phenomenon_name,
            property_set_name,
            clock,
            static_cast<lue::data_model::Count>(nr_time_steps),
            {0, static_cast<lue::data_model::Count>(nr_time_steps)},
            {static_cast<lue::data_model::Count>(array_shape[0]),
             static_cast<lue::data_model::Count>(array_shape[1])},
            space_box);

        std::string const property_basename = "property";
        std::vector<std::string> property_names(nr_rasters);

        for (lue::Count raster_idx = 0; raster_idx < nr_rasters; ++raster_idx)
        {
            std::string const property_name = std::format("{}-{}", property_basename, raster_idx);
            view.add_layer<Element>(property_name);
            property_names[raster_idx] = property_name;
        }

        return {view.object_id(), property_names};
    }

}  // Anonymous namespace


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

    using Element = lue::LargestIntegralElement;

    auto const [object_id, array_shape, partition_shape] = layout_constant_raster<Element>(array_pathname);

    // Create, write, read, and compare arrays
    Array<Element> array_written =
        lue::value_policies::uniform<Element>(array_shape, partition_shape, Element{0}, Element{10});
    hpx::future<void> write_finished = lue::to_lue(array_written, array_pathname, object_id);

#if !BOOST_OS_WINDOWS
    write_finished.wait();
#endif

    Array<Element> array_read = lue::from_lue<Element>(array_pathname, partition_shape, object_id);

    lue::test::check_arrays_are_equal(array_read, array_written);
}


BOOST_AUTO_TEST_CASE(variable_raster)
{
    // Write a variable raster with integers and read them back in. Compare rasters written with rasters
    // read.
    namespace ldm = lue::data_model;

    std::string const dataset_pathname{"lue_framework_io_lue_variable_raster.lue"};
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"area"};
    std::string const layer_name{"elevation"};
    std::string const array_pathname{
        std::format("{}/{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name, layer_name)};

    using Element = lue::LargestIntegralElement;

    auto const [object_id, nr_time_steps, array_shape, partition_shape] =
        layout_variable_raster<Element>(array_pathname);

    // Create, write, read, and compare arrays
    for (lue::Index time_step_idx = 0; time_step_idx < nr_time_steps; ++time_step_idx)
    {
        Array<Element> array_written =
            lue::value_policies::uniform<Element>(array_shape, partition_shape, Element{0}, Element{10});

        hpx::future<void> write_finished =
            lue::to_lue(array_written, array_pathname, object_id, time_step_idx);

#if !BOOST_OS_WINDOWS
        write_finished.wait();
#endif

        Array<Element> array_read =
            lue::from_lue<Element>(array_pathname, partition_shape, object_id, time_step_idx);

        lue::test::check_arrays_are_equal(array_read, array_written);
    }
}


BOOST_AUTO_TEST_CASE(multiple_read_write_constant_raster_same_file_1)
{
    // 1. Create n arrays
    // 2. Write n arrays
    // 3. Read n arrays
    // 4. Compare n arrays read with ones written
    namespace ldm = lue::data_model;

    std::string const dataset_pathname{
        "lue_framework_io_lue_multiple_read_write_constant_raster_same_file_1.lue"};
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"area"};

    auto const [array_shape, partition_shape] = layout_raster();
    lue::Count const nr_properties = 5;

    using Element = lue::LargestIntegralElement;

    std::string const property_set_pathname{
        std::format("{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name)};
    auto const [object_id, property_names] =
        layout_constant_rasters<Element>(property_set_pathname, array_shape, nr_properties);

    // Create arrays
    std::vector<Array<Element>> arrays_written(nr_properties);

    for (lue::Index property_idx = 0; property_idx < nr_properties; ++property_idx)
    {
        arrays_written[property_idx] =
            lue::value_policies::uniform<Element>(array_shape, partition_shape, Element{0}, Element{10});
    }

    // Write arrays
    std::vector<std::string> array_pathnames(nr_properties);
    std::vector<hpx::future<void>> writes_finished(nr_properties);

    for (lue::Index property_idx = 0; property_idx < nr_properties; ++property_idx)
    {
        std::string const& property_name = property_names[property_idx];
        std::string const array_pathname{std::format("{}/{}", property_set_pathname, property_name)};
        array_pathnames[property_idx] = array_pathname;

        writes_finished[property_idx] =
            lue::to_lue(arrays_written[property_idx], array_pathnames[property_idx], object_id);
    }

#if !BOOST_OS_WINDOWS
    hpx::wait_all(writes_finished);
#endif

    // Read arrays
    std::vector<Array<Element>> arrays_read(nr_properties);

    for (lue::Index property_idx = 0; property_idx < nr_properties; ++property_idx)
    {
        arrays_read[property_idx] =
            lue::from_lue<Element>(array_pathnames[property_idx], partition_shape, object_id);
    }

    // Compare arrays
    for (lue::Index property_idx = 0; property_idx < nr_properties; ++property_idx)
    {
        lue::test::check_arrays_are_equal(arrays_read[property_idx], arrays_written[property_idx]);
    }
}


BOOST_AUTO_TEST_CASE(multiple_read_write_variable_raster_same_file_1)
{
    // 1. Create n stacks of t arrays
    // 2. Write n stacks of t arrays
    // 3. Read n stacks of t arrays
    // 4. Compare n stacks of t arrays read with ones written
    namespace ldm = lue::data_model;

    std::string const dataset_pathname{"lue_framework_io_lue_multiple_read_variable_raster_same_file_1.lue"};
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"area"};

    auto const [array_shape, partition_shape] = layout_raster();
    lue::Count const nr_time_steps = 15;
    lue::Count const nr_properties = 5;

    using Element = lue::LargestIntegralElement;

    std::string const property_set_pathname{
        std::format("{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name)};
    auto const [object_id, property_names] =
        layout_variable_rasters<Element>(property_set_pathname, nr_time_steps, array_shape, nr_properties);

    // Create stacks of arrays
    std::vector<std::vector<Array<Element>>> array_stacks_written(nr_properties);

    for (lue::Index property_idx = 0; property_idx < nr_properties; ++property_idx)
    {
        array_stacks_written[property_idx] = std::vector<Array<Element>>(nr_time_steps);

        for (lue::Index time_step_idx = 0; time_step_idx < nr_time_steps; ++time_step_idx)
        {
            array_stacks_written[property_idx][time_step_idx] =
                lue::value_policies::uniform<Element>(array_shape, partition_shape, Element{0}, Element{10});
        }
    }

    // Write arrays
    std::vector<std::string> array_pathnames(nr_properties);
    std::vector<std::vector<hpx::future<void>>> writes_finished(nr_properties);

    for (lue::Index property_idx = 0; property_idx < nr_properties; ++property_idx)
    {
        std::string const& property_name = property_names[property_idx];
        std::string const array_pathname{std::format("{}/{}", property_set_pathname, property_name)};
        array_pathnames[property_idx] = array_pathname;

        writes_finished[property_idx] = std::vector<hpx::future<void>>(nr_time_steps);

        for (lue::Index time_step_idx = 0; time_step_idx < nr_time_steps; ++time_step_idx)
        {
            writes_finished[property_idx][time_step_idx] = lue::to_lue(
                array_stacks_written[property_idx][time_step_idx],
                array_pathnames[property_idx],
                object_id,
                time_step_idx);
        }
    }

#if !BOOST_OS_WINDOWS
    for (lue::Index property_idx = 0; property_idx < nr_properties; ++property_idx)
    {
        hpx::wait_all(writes_finished[property_idx]);
    }
#endif

    // Read arrays
    std::vector<std::vector<Array<Element>>> arrays_read(nr_properties);

    for (lue::Index property_idx = 0; property_idx < nr_properties; ++property_idx)
    {
        arrays_read[property_idx] = std::vector<Array<Element>>(nr_time_steps);

        for (lue::Index time_step_idx = 0; time_step_idx < nr_time_steps; ++time_step_idx)
        {
            arrays_read[property_idx][time_step_idx] = lue::from_lue<Element>(
                array_pathnames[property_idx], partition_shape, object_id, time_step_idx);
        }
    }

    // Compare arrays
    for (lue::Index property_idx = 0; property_idx < nr_properties; ++property_idx)
    {
        for (lue::Index time_step_idx = 0; time_step_idx < nr_time_steps; ++time_step_idx)
        {
            lue::test::check_arrays_are_equal(
                arrays_read[property_idx][time_step_idx], array_stacks_written[property_idx][time_step_idx]);
        }
    }
}


BOOST_AUTO_TEST_CASE(multiple_read_write_constant_raster_same_file_2)
{
    // Do this n times:
    // - Create an array
    // - Write an array
    // - Read an array
    // - Compare the arrays

    namespace ldm = lue::data_model;

    std::string const dataset_pathname{"lue_framework_io_lue_multiple_read_constant_raster_same_file_2.lue"};
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"area"};

    auto const [array_shape, partition_shape] = layout_raster();
    lue::Count const nr_properties = 5;

    using Element = lue::LargestIntegralElement;

    std::string const property_set_pathname{
        std::format("{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name)};
    auto const [object_id, property_names] =
        layout_constant_rasters<Element>(property_set_pathname, array_shape, nr_properties);

    for (lue::Index property_idx = 0; property_idx < nr_properties; ++property_idx)
    {
        // Create array
        Array<Element> const array_written =
            lue::value_policies::uniform<Element>(array_shape, partition_shape, Element{0}, Element{10});

        // Write array
        std::string const& property_name = property_names[property_idx];
        std::string const array_pathname{std::format("{}/{}", property_set_pathname, property_name)};
        hpx::future<void> write_finished = lue::to_lue(array_written, array_pathname, object_id);

#if !BOOST_OS_WINDOWS
        write_finished.wait();
#endif

        // Read array
        Array<Element> const array_read = lue::from_lue<Element>(array_pathname, partition_shape, object_id);

        // Compare arrays
        lue::test::check_arrays_are_equal(array_read, array_written);
    }
}


BOOST_AUTO_TEST_CASE(multiple_read_write_variable_raster_same_file_2)
{
    // Do this n times:
    // - Create a stack of t arrays
    // - Write a stack of t arrays
    // - Read a stack of t arrays
    // - Compare the stack of t arrays

    namespace ldm = lue::data_model;

    std::string const dataset_pathname{"lue_framework_io_lue_multiple_read_variable_raster_same_file_2.lue"};
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"area"};
    // std::string const property_name{"elevation"};
    // std::string const array_pathname{
    //     std::format("{}/{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name, property_name)};

    auto const [array_shape, partition_shape] = layout_raster();
    lue::Count const nr_time_steps = 15;
    lue::Count const nr_properties = 5;

    using Element = lue::LargestIntegralElement;

    std::string const property_set_pathname{
        std::format("{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name)};
    auto const [object_id, property_names] =
        layout_variable_rasters<Element>(property_set_pathname, nr_time_steps, array_shape, nr_properties);

    for (lue::Index property_idx = 0; property_idx < nr_properties; ++property_idx)
    {
        // Create stack of arrays
        std::vector<Array<Element>> array_stacks_written(nr_time_steps);

        for (lue::Index time_step_idx = 0; time_step_idx < nr_time_steps; ++time_step_idx)
        {
            array_stacks_written[time_step_idx] =
                lue::value_policies::uniform<Element>(array_shape, partition_shape, Element{0}, Element{10});
        }

        // Write stack of arrays
        std::vector<hpx::future<void>> writes_finished(nr_time_steps);
        std::string const& property_name = property_names[property_idx];
        std::string const array_pathname{std::format("{}/{}", property_set_pathname, property_name)};

        for (lue::Index time_step_idx = 0; time_step_idx < nr_time_steps; ++time_step_idx)
        {
            writes_finished[time_step_idx] =
                lue::to_lue(array_stacks_written[time_step_idx], array_pathname, object_id, time_step_idx);
        }

#if !BOOST_OS_WINDOWS
        hpx::wait_all(writes_finished);
#endif

        // Read arrays
        std::vector<Array<Element>> arrays_read(nr_time_steps);

        for (lue::Index time_step_idx = 0; time_step_idx < nr_time_steps; ++time_step_idx)
        {
            arrays_read[time_step_idx] =
                lue::from_lue<Element>(array_pathname, partition_shape, object_id, time_step_idx);
        }

        // Compare arrays
        for (lue::Index time_step_idx = 0; time_step_idx < nr_time_steps; ++time_step_idx)
        {
            lue::test::check_arrays_are_equal(
                arrays_read[time_step_idx], array_stacks_written[time_step_idx]);
        }
    }
}
