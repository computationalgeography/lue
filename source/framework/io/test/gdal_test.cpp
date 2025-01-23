#define BOOST_TEST_MODULE lue framework io gdal
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/range.hpp"
#include "lue/framework/algorithm/value_policies/add.hpp"
#include "lue/framework/algorithm/value_policies/cell_index.hpp"
#include "lue/framework/io/gdal.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"
#include "lue/gdal.hpp"
#include <hpx/config.hpp>


BOOST_AUTO_TEST_CASE(array_all_valid)
{
    // Signed int, all valid
    using Element = lue::SignedIntegralElement<0>;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{5};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_gdal_array_int_all_valid.tif"};

    lue::to_gdal<Element>(array_written, name).wait();

    BOOST_CHECK(lue::gdal::try_open_dataset(name, GDALAccess::GA_ReadOnly));

    Array array_read{lue::from_gdal<Element>(name, partition_shape)};

    lue::test::check_arrays_are_equal(array_read, array_written);
}


BOOST_AUTO_TEST_CASE(array_none_valid)
{
    // Signed int, none valid
    using Element = lue::SignedIntegralElement<0>;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{lue::policy::no_data_value<Element>};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_gdal_array_int_none_valid.tif"};

    lue::to_gdal<Element>(array_written, name).wait();

    BOOST_CHECK(lue::gdal::try_open_dataset(name, GDALAccess::GA_ReadOnly));

    Array array_read{lue::from_gdal<Element>(name, partition_shape)};

    lue::test::check_arrays_are_equal(array_read, array_written);
}


BOOST_AUTO_TEST_CASE(array_float_all_valid)
{
    // Float, all valid
    using Element = lue::FloatingPointElement<0>;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{5};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_gdal_array_float_all_valid.tif"};

    lue::to_gdal<Element>(array_written, name).wait();

    BOOST_CHECK(lue::gdal::try_open_dataset(name, GDALAccess::GA_ReadOnly));

    Array array_read{lue::from_gdal<Element>(name, partition_shape)};

    lue::test::check_arrays_are_equal(array_read, array_written);
}


BOOST_AUTO_TEST_CASE(array_float_none_valid)
{
    // Float, none valid
    using Element = lue::FloatingPointElement<0>;
    using Array = lue::PartitionedArray<Element, 2>;
    lue::ShapeT<Array> array_shape{60, 40};
    lue::ShapeT<Array> partition_shape{10, 10};
    Element const fill_value{lue::policy::no_data_value<Element>};

    Array array_written{lue::create_partitioned_array<Element>(array_shape, partition_shape, fill_value)};
    std::string const name{"lue_framework_io_gdal_array_float_none_valid.tif"};

    lue::to_gdal<Element>(array_written, name).wait();

    BOOST_CHECK(lue::gdal::try_open_dataset(name, GDALAccess::GA_ReadOnly));

    Array array_read{lue::from_gdal<Element>(name, partition_shape)};

    lue::test::check_arrays_are_equal(array_read, array_written);
}


template<typename Element>
using Array = lue::PartitionedArray<Element, 2>;


BOOST_AUTO_TEST_CASE(hyperslab)
{
    using namespace lue::value_policies;

    using ConditionArray = Array<lue::BooleanElement>;
    using IndexArray = Array<lue::IndexElement>;
    using Shape = lue::ShapeT<ConditionArray>;
    using Hyperslab = lue::Hyperslab<2>;

    Shape const array_shape{60, 40};
    Shape const partition_shape{10, 10};

    ConditionArray const condition{
        lue::create_partitioned_array<lue::BooleanElement>(array_shape, partition_shape, 1)};
    IndexArray const row_idxs_written = cell_index<lue::IndexElement>(condition, 0);
    IndexArray const col_idxs_written = cell_index<lue::IndexElement>(condition, 1);

    std::string const row_idxs_name{"lue_framework_io_gdal_hyperslab_row_idxs.tif"};
    std::string const col_idxs_name{"lue_framework_io_gdal_hyperslab_col_idxs.tif"};

    lue::to_gdal(row_idxs_written, row_idxs_name).wait();
    lue::to_gdal(col_idxs_written, col_idxs_name).wait();

    Hyperslab const hyperslab{{30, 20}, {20, 10}};

    IndexArray const row_idxs_read{
        lue::from_gdal<lue::IndexElement>(row_idxs_name, hyperslab, partition_shape)};
    IndexArray const col_idxs_read{
        lue::from_gdal<lue::IndexElement>(col_idxs_name, hyperslab, partition_shape)};

    ConditionArray const hyperslab_condition{
        lue::create_partitioned_array<lue::BooleanElement>(hyperslab.counts(), partition_shape, 1)};
    IndexArray const hyperslab_row_idxs_written = cell_index<lue::IndexElement>(hyperslab_condition, 0) +
                                                  static_cast<lue::IndexElement>(hyperslab.offsets()[0]);
    IndexArray const hyperslab_col_idxs_written = cell_index<lue::IndexElement>(hyperslab_condition, 1) +
                                                  static_cast<lue::IndexElement>(hyperslab.offsets()[1]);

    lue::test::check_arrays_are_equal(row_idxs_read, hyperslab_row_idxs_written);
    lue::test::check_arrays_are_equal(col_idxs_read, hyperslab_col_idxs_written);
}


BOOST_AUTO_TEST_CASE(valid_hyperslab)
{
    using namespace lue::value_policies;

    using ConditionArray = Array<lue::BooleanElement>;
    using IndexArray = Array<lue::IndexElement>;
    using Shape = lue::ShapeT<ConditionArray>;
    using Hyperslab = lue::Hyperslab<2>;

    Shape const array_shape{60, 40};
    Shape const partition_shape{10, 10};

    ConditionArray const condition{
        lue::create_partitioned_array<lue::BooleanElement>(array_shape, partition_shape, 1)};
    IndexArray const idxs_written = cell_index<lue::IndexElement>(condition, 0);

    std::string const idxs_name{"lue_framework_io_gdal_wrong_hyperslab_idxs.tif"};

    lue::to_gdal(idxs_written, idxs_name).wait();

    {
        // Hyperslab corresponds with whole array
        Hyperslab const hyperslab{{30, 20}, {60, 40}};
        BOOST_CHECK_NO_THROW(lue::from_gdal<lue::IndexElement>(idxs_name, hyperslab, partition_shape));
    }
}


BOOST_AUTO_TEST_CASE(incorrect_hyperslab)
{
    using namespace lue::value_policies;

    using ConditionArray = Array<lue::BooleanElement>;
    using IndexArray = Array<lue::IndexElement>;
    using Shape = lue::ShapeT<ConditionArray>;
    using Hyperslab = lue::Hyperslab<2>;

    Shape const array_shape{60, 40};
    Shape const partition_shape{10, 10};

    ConditionArray const condition{
        lue::create_partitioned_array<lue::BooleanElement>(array_shape, partition_shape, 1)};
    IndexArray const idxs_written = cell_index<lue::IndexElement>(condition, 0);

    std::string const idxs_name{"lue_framework_io_gdal_wrong_hyperslab_idxs.tif"};

    lue::to_gdal(idxs_written, idxs_name).wait();

    {
        // Empty hyperslab
        Hyperslab const hyperslab{{30, 20}, {0, 0}};
        BOOST_CHECK_EXCEPTION(
            lue::from_gdal<lue::IndexElement>(idxs_name, hyperslab, partition_shape),
            std::runtime_error,
            [](auto const& exception)
            { return std::string(exception.what()).find("shape must not be empty") != std::string::npos; });
    }

    {
        // Empty hyperslab
        Hyperslab const hyperslab{{30, 20}, {20, 0}};
        BOOST_CHECK_EXCEPTION(
            lue::from_gdal<lue::IndexElement>(idxs_name, hyperslab, partition_shape),
            std::runtime_error,
            [](auto const& exception)
            { return std::string(exception.what()).find("shape must not be empty") != std::string::npos; });
    }

    {
        // Empty hyperslab
        Hyperslab const hyperslab{{30, 20}, {0, 10}};
        BOOST_CHECK_EXCEPTION(
            lue::from_gdal<lue::IndexElement>(idxs_name, hyperslab, partition_shape),
            std::runtime_error,
            [](auto const& exception)
            { return std::string(exception.what()).find("shape must not be empty") != std::string::npos; });
    }

    {
        // Hyperslab too large
        Hyperslab const hyperslab{{55, 35}, {20, 20}};

        BOOST_CHECK_EXCEPTION(
            lue::from_gdal<lue::IndexElement>(idxs_name, hyperslab, partition_shape),
            std::runtime_error,
            [](auto const& exception)
            { return std::string(exception.what()).find("extents beyond array") != std::string::npos; });
    }
}
