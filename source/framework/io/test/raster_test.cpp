#define BOOST_TEST_MODULE lue framework io raster
#include <hpx/config.hpp>
#include "lue/framework/io/gdal.hpp"
#include "lue/framework/io/raster.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include <numeric>


BOOST_AUTO_TEST_CASE(read_1)
{
    using Element = std::int32_t;
    std::string const name{"read_1.tif"};
    int const nr_rows{60};
    int const nr_cols{40};
    int const nr_bands{1};
    GDALDataType const data_type{lue::GDALTypeTraits<Element>::type_id};

    std::vector<Element> cells(nr_rows * nr_cols);
    std::iota(cells.begin(), cells.end(), 0);

    lue::register_gdal_drivers();

    // Create an in-memory GDAL raster, read it, and verify the contents match
    {
        // TODO Configure a certain value as no-data value
        //      Upon reading, this value must be detected by the input no-data policy and the
        //      output no-data policy must be used to write the corresponding cell in the array.

        lue::GDALDatasetPtr dataset_ptr{lue::create(name, {nr_rows, nr_cols}, nr_bands, data_type)};

        lue::GDALBandPtr band_ptr{lue::get_raster_band(*dataset_ptr)};

        CPLErr status = band_ptr->RasterIO(
            GF_Write, 0, 0, nr_cols, nr_rows, cells.data(), nr_cols, nr_rows, data_type, 0, 0, nullptr);
        BOOST_REQUIRE(status != CE_Failure);
    }

    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> partition_shape{10, 10};

    Array array_we_got = lue::read<Element>(name, partition_shape);

    lue::ShapeT<Array> array_shape{nr_rows, nr_cols};

    Array array_we_want = lue::create_partitioned_array<Element>(array_shape, partition_shape, 5);

    hpx::future<void> written = lue::write<Element>(array_we_got, "written.tif", name);

    written.wait();


    // lue::test::check_arrays_are_equal(array_we_got, array_we_want);

    // TODO Verify contents are as expected
    // TODO Handle no-data in input raster
    // TODO Create Python wrappers
    //     - Test value type of raster
    //     - Call corresponding read. Don't cast. This can be added later, if needed.



    // TODO Make it easy to test different arrays
    // TODO Test floating point array with NaN no-data
    // (TODO Test floating point array with non-NaN)
}


// BOOST_AUTO_TEST_CASE(write_1)
// {
//     BOOST_CHECK(false);
// }
