#define BOOST_TEST_MODULE lue framework api cxx local add
#include "lue/framework/api/cxx/operator.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(add_raster_raster)
{
    using Element = std::int32_t;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const raster1 = Array{};
    lue::api::Field const raster2 = Array{};

    {
        lue::api::Field result = lue::api::add(raster1, raster2);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = raster1 + raster2;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(add_raster_scalar)
{
    using Element = std::int32_t;
    using Array = lue::PartitionedArray<Element, 2>;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const raster = Array{};
    lue::api::Field const scalar = Scalar{};

    {
        lue::api::Field result = lue::api::add(raster, scalar);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = raster + scalar;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(add_scalar_raster)
{
    using Element = std::int32_t;
    using Scalar = lue::Scalar<Element>;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const scalar = Scalar{};
    lue::api::Field const raster = Array{};

    {
        lue::api::Field result = lue::api::add(scalar, raster);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = scalar + raster;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(add_scalar_scalar)
{
    using Element = std::int32_t;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar1 = Scalar{};
    lue::api::Field const scalar2 = Scalar{};

    {
        lue::api::Field result = lue::api::add(scalar1, scalar2);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = scalar1 + scalar2;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(add_raster_value)
{
    using Element = std::int32_t;
    using Array = lue::PartitionedArray<Element, 2>;
    using Element = Element;

    lue::api::Field const raster = Array{};
    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::add(raster, value);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = raster + value;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(add_value_raster)
{
    using Element = std::int32_t;
    using Array = lue::PartitionedArray<std::int32_t, 2>;

    lue::api::Field const value = Element{};
    lue::api::Field const raster = Array{};

    {
        lue::api::Field result = lue::api::add(value, raster);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = value + raster;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(add_scalar_value)
{
    using Element = std::int32_t;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar = Scalar{};
    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::add(scalar, value);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = scalar + value;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(add_value_scalar)
{
    using Element = std::int32_t;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar = Scalar{};
    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::add(value, scalar);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = value + scalar;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(add_value_value)
{
    using Element = std::int32_t;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const value1 = Element{};
    lue::api::Field const value2 = Element{};

    {
        lue::api::Field result = lue::api::add(value1, value2);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = value1 + value2;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}
