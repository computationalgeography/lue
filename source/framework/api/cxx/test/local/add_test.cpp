#define BOOST_TEST_MODULE lue framework api cxx local add
#include "lue/framework/api/cxx/local/add.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(add_raster_raster)
{
    using Raster = lue::PartitionedArray<std::int32_t, 2>;

    lue::Field raster1 = Raster{};
    lue::Field raster2 = Raster{};

    lue::Field result = lue::add(raster1, raster2);

    // TODO test result is a Raster as well
}


BOOST_AUTO_TEST_CASE(add_raster_scalar)
{
    using Raster = lue::PartitionedArray<std::int32_t, 2>;
    using Scalar = lue::Scalar<std::int32_t>;

    lue::Field raster = Raster{};
    lue::Field scalar = Scalar{};

    lue::Field result = lue::add(raster, scalar);

    // TODO test result is a Raster as well
}


BOOST_AUTO_TEST_CASE(add_scalar_raster)
{
    using Scalar = lue::Scalar<std::int32_t>;
    using Raster = lue::PartitionedArray<std::int32_t, 2>;

    lue::Field scalar = Scalar{};
    lue::Field raster = Raster{};

    lue::Field result = lue::add(scalar, raster);

    // TODO test result is a Raster as well
}


BOOST_AUTO_TEST_CASE(add_scalar_scalar)
{
    using Scalar = lue::Scalar<std::int32_t>;

    lue::Field scalar1 = Scalar{};
    lue::Field scalar2 = Scalar{};

    lue::Field result = lue::add(scalar1, scalar2);

    // TODO test result is a Scalar as well
}


BOOST_AUTO_TEST_CASE(add_raster_value)
{
    using Raster = lue::PartitionedArray<std::int32_t, 2>;
    using Element = std::int32_t;

    lue::Field raster = Raster{};
    lue::Field value = Element{};

    lue::Field result = lue::add(raster, value);

    // TODO test result is a Raster as well
}


BOOST_AUTO_TEST_CASE(add_value_raster)
{
    using Element = std::int32_t;
    using Raster = lue::PartitionedArray<std::int32_t, 2>;

    lue::Field value = Element{};
    lue::Field raster = Raster{};

    lue::Field result = lue::add(value, raster);

    // TODO test result is a Raster as well
}


BOOST_AUTO_TEST_CASE(add_scalar_value)
{
    using Scalar = lue::Scalar<std::int32_t>;
    using Element = std::int32_t;

    lue::Field scalar = Scalar{};
    lue::Field value = Element{};

    lue::Field result = lue::add(scalar, value);

    // TODO test result is a Scalar as well
}


BOOST_AUTO_TEST_CASE(add_value_scalar)
{
    using Element = std::int32_t;
    using Scalar = lue::Scalar<std::int32_t>;

    lue::Field scalar = Scalar{};
    lue::Field value = Element{};

    lue::Field result = lue::add(value, scalar);

    // TODO test result is a Scalar as well
}


BOOST_AUTO_TEST_CASE(add_value_value)
{
    using Element = std::int32_t;

    lue::Field value1 = Element{};
    lue::Field value2 = Element{};

    lue::Field result = lue::add(value1, value2);

    // TODO test result is a Scalar
}
