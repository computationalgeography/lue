#define BOOST_TEST_MODULE lue framework io lue
#include "lue/framework/algorithm/value_policies/uniform.hpp"
#include "lue/framework/io/from_lue.hpp"
#include "lue/framework/io/to_lue.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/data_model/hl/raster_view.hpp"
#include "lue/framework.hpp"
#include <hpx/config.hpp>
#include <boost/predef.h>


// TODO:
// - Add tests for constant rasters. Requires update in data model code
// - Revamp parallel I/O logic


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
    auto layout_constant_raster(std::string const& array_pathname) -> std::tuple<ObjectID, Shape, Shape>
    {
        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
            lue::parse_array_pathname(array_pathname);

        using NrElements = lue::LargestIntegralElement;

        lue::data_model::Count const nr_rows_partition{static_cast<lue::data_model::Count>(
            lue::value_policies::uniform<NrElements>(10, 100).future().get())};
        lue::data_model::Count const nr_cols_partition{static_cast<lue::data_model::Count>(
            lue::value_policies::uniform<NrElements>(10, 100).future().get())};
        Shape const partition_shape{
            static_cast<lue::Count>(nr_rows_partition), static_cast<lue::Count>(nr_cols_partition)};

        lue::data_model::Count const nr_rows{static_cast<lue::data_model::Count>(
            lue::value_policies::uniform<NrElements>(100, 1000).future().get())};
        lue::data_model::Count const nr_cols{static_cast<lue::data_model::Count>(
            lue::value_policies::uniform<NrElements>(100, 1000).future().get())};
        lue::hdf5::Shape const raster_shape{nr_rows, nr_cols};

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
            dataset_ptr, phenomenon_name, property_set_name, raster_shape, space_box);

        view.add_layer<Element>(property_name);

        Shape const grid_shape{
            static_cast<lue::Count>(raster_shape[0]), static_cast<lue::Count>(raster_shape[1])};

        return {view.object_id(), grid_shape, partition_shape};
    }


    template<typename Element>
    auto layout_variable_raster(std::string const& array_pathname)
        -> std::tuple<ObjectID, lue::Count, Shape, Shape>
    {
        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
            lue::parse_array_pathname(array_pathname);

#if 1
        using NrElements = lue::LargestIntegralElement;

        lue::Count const nr_rows{
            static_cast<lue::Count>(lue::value_policies::uniform<NrElements>(100, 1000).future().get())};
        lue::Count const nr_cols{
            static_cast<lue::Count>(lue::value_policies::uniform<NrElements>(100, 1000).future().get())};
        lue::Count const nr_rows_partition{
            static_cast<lue::Count>(lue::value_policies::uniform<NrElements>(10, 100).future().get())};
        lue::Count const nr_cols_partition{
            static_cast<lue::Count>(lue::value_policies::uniform<NrElements>(10, 100).future().get())};
        lue::Count const nr_time_steps{
            static_cast<lue::Count>(lue::value_policies::uniform<NrElements>(5, 20).future().get())};
#else
        lue::Count const nr_rows{60};
        lue::Count const nr_cols{40};
        lue::Count const nr_rows_partition{5};
        lue::Count const nr_cols_partition{5};
        lue::Count const nr_time_steps{20};
#endif

        Shape const raster_shape{nr_rows, nr_cols};
        Shape const partition_shape{nr_rows_partition, nr_cols_partition};

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
            {static_cast<lue::data_model::Count>(raster_shape[0]),
             static_cast<lue::data_model::Count>(raster_shape[1])},
            space_box);

        view.add_layer<Element>(property_name);

        return {view.object_id(), nr_time_steps, raster_shape, partition_shape};
    }

}  // Anonymous namespace


#if 0
BOOST_AUTO_TEST_CASE(constant_raster)
{
    // TODO: make this work as variable rasters

    // Write a constant raster with integers and read it back in. Compare raster written with raster read.
    namespace ldm = lue::data_model;

    std::string const dataset_pathname{"lue_framework_io_lue_constant_raster.lue"};
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"area"};
    std::string const layer_name{"elevation"};
    std::string const array_pathname{
        std::format("{}/{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name, layer_name)};

    using Element = lue::LargestIntegralElement;

    auto const [object_id, raster_shape, partition_shape] = layout_constant_raster<Element>(array_pathname);

    // Create, write, read, and compare arrays
    Array<Element> array_written =
        lue::value_policies::uniform<Element>(raster_shape, partition_shape, Element{0}, Element{10});
    hpx::future<void> write_finished = lue::to_lue(array_written, array_pathname, object_id);

    // TODO: See variable_raster test case
    write_finished.get();

    Array<Element> array_read = lue::from_lue<Element>(array_pathname, partition_shape, object_id);
    lue::test::check_arrays_are_equal(array_read, array_written);
}
#endif


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

    auto const [object_id, nr_time_steps, raster_shape, partition_shape] =
        layout_variable_raster<Element>(array_pathname);

    // Create, write, read, and compare arrays
    for (lue::Index time_step = 0; time_step < nr_time_steps; ++time_step)
    {
        Array<Element> array_written =
            lue::value_policies::uniform<Element>(raster_shape, partition_shape, Element{0}, Element{10});
        hpx::future<void> write_finished = lue::to_lue(array_written, array_pathname, object_id, time_step);

        // TODO:
        // Waiting for the write to finish here fixes an error from occurring in ~5% of the cases:

        // clang-format off
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
        // clang-format on
        //
        // The error suggests that to_lue can't open the dataset because from_lue is not ready reading from
        // it.
        //
        // Waiting for to_lue to finish here prevents the issue. Weird thing is that moving these
        // synchronization points elsewhere, like into to_lue or from_lue does not prevent the issue.
        // See https://github.com/computationalgeography/lue/issues/945

#ifndef BOOST_OS_WINDOWS
        // write_finished.get();  // Also fine
        lue::detail::to_lue_finished(lue::detail::normalize(dataset_pathname), time_step + 1).wait();
#endif

        Array<Element> array_read =
            lue::from_lue<Element>(array_pathname, partition_shape, object_id, time_step);

        lue::test::check_arrays_are_equal(array_read, array_written);
    }
}


// TODO:
BOOST_AUTO_TEST_CASE(multiple_read_write_constant_raster_same_file_1)
{
}


// TODO:
BOOST_AUTO_TEST_CASE(multiple_read_write_constant_raster_same_file_2)
{
}


BOOST_AUTO_TEST_CASE(multiple_read_write_variable_raster_same_file_1)
{
    // 1. Create stack of n arrays
    // 2. Write arrays
    // 3. Read arrays
    // 4. Compare arrays read with ones written
    namespace ldm = lue::data_model;

    std::string const dataset_pathname{"lue_framework_io_lue_multiple_read_variable_raster_same_file_1.lue"};
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"area"};
    std::string const property_name{"elevation"};
    std::string const array_pathname{
        std::format("{}/{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name, property_name)};

    using Element = lue::LargestIntegralElement;

    auto const [object_id, nr_time_steps, raster_shape, partition_shape] =
        layout_variable_raster<Element>(array_pathname);

    // Create arrays
    std::vector<Array<Element>> arrays_written(nr_time_steps);
    for (lue::Index time_step = 0; time_step < static_cast<lue::Count>(nr_time_steps); ++time_step)
    {
        arrays_written[time_step] =
            lue::value_policies::uniform<Element>(raster_shape, partition_shape, Element{0}, Element{10});
    }

    // Write arrays
    std::vector<hpx::future<void>> writes_finished(nr_time_steps);
    for (lue::Index time_step = 0; time_step < static_cast<lue::Count>(nr_time_steps); ++time_step)
    {
        writes_finished[time_step] =
            lue::to_lue(arrays_written[time_step], array_pathname, object_id, time_step);
    }

#ifndef BOOST_OS_WINDOWS
    // TODO: See variable_raster test case
    writes_finished.back().wait();
#endif

    // Read arrays
    std::vector<Array<Element>> arrays_read(nr_time_steps);
    for (lue::Index time_step = 0; time_step < static_cast<lue::Count>(nr_time_steps); ++time_step)
    {
        arrays_read[time_step] =
            lue::from_lue<Element>(array_pathname, partition_shape, object_id, time_step);
    }

    // Compare arrays
    for (lue::Index time_step = 0; time_step < static_cast<lue::Count>(nr_time_steps); ++time_step)
    {
        lue::test::check_arrays_are_equal(arrays_read[time_step], arrays_written[time_step]);
    }
}


BOOST_AUTO_TEST_CASE(multiple_read_write_variable_raster_same_file_2)
{
    // Iteratively write, read, and compare n arrays
    namespace ldm = lue::data_model;

    std::string const dataset_pathname{"lue_framework_io_lue_multiple_read_variable_raster_same_file_2.lue"};
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"area"};
    std::string const property_name{"elevation"};
    std::string const array_pathname{
        std::format("{}/{}/{}/{}", dataset_pathname, phenomenon_name, property_set_name, property_name)};

    using Element = lue::LargestIntegralElement;

    auto const [object_id, nr_time_steps, raster_shape, partition_shape] =
        layout_variable_raster<Element>(array_pathname);

    // Create, write, read, and compare arrays
    for (lue::Index time_step = 0; time_step < static_cast<lue::Count>(nr_time_steps); ++time_step)
    {
        Array<Element> array_written =
            lue::value_policies::uniform<Element>(raster_shape, partition_shape, Element{0}, Element{10});

        hpx::future<void> write_finished = lue::to_lue(array_written, array_pathname, object_id, time_step);

#ifndef BOOST_OS_WINDOWS
        // TODO: See variable_raster test case
        write_finished.wait();
#endif

        Array<Element> array_read =
            lue::from_lue<Element>(array_pathname, partition_shape, object_id, time_step);

        lue::test::check_arrays_are_equal(array_read, array_written);
    }
}
