#define BOOST_TEST_MODULE lue framework api cxx local create_array
#include "lue/framework/api/cxx/create_array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(create_array)
{
    using Element = std::int32_t;
    using Raster = lue::PartitionedArray<Element, 2>;
    using Shape = lue::ShapeT<Raster>;

    Shape const array_shape{1, 1};
    Shape const partition_shape{1, 1};

    lue::api::Scalar const fill_value = Element{};

    lue::api::Field result = lue::api::create_array(array_shape, partition_shape, fill_value);

    BOOST_CHECK(std::holds_alternative<Raster>(result));
}


BOOST_AUTO_TEST_CASE(create_array_without_partition_shape)
{
    using Element = std::int32_t;
    using Raster = lue::PartitionedArray<Element, 2>;
    using Shape = lue::ShapeT<Raster>;

    Shape const array_shape{1, 1};

    lue::api::Scalar const fill_value = Element{};

    lue::api::Field result = lue::api::create_array(array_shape, fill_value);

    BOOST_CHECK(std::holds_alternative<Raster>(result));
}
