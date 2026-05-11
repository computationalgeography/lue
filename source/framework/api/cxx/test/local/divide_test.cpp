#define BOOST_TEST_MODULE lue framework api cxx local divide
#include "lue/framework/api/cxx/operator.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(divide_raster_raster)
{
    using Element = lue::LargestFloatingPointElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const raster1 = Array{};
    lue::api::Field const raster2 = Array{};

    {
        lue::api::Field result = lue::api::divide(raster1, raster2);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = raster1 / raster2;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(divide_raster_scalar)
{
    using Element = lue::LargestFloatingPointElement;
    using Array = lue::PartitionedArray<Element, 2>;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const raster = Array{};
    lue::api::Field const scalar = Scalar{};

    {
        lue::api::Field result = lue::api::divide(raster, scalar);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = raster / scalar;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(divide_scalar_raster)
{
    using Element = lue::LargestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const scalar = Scalar{};
    lue::api::Field const raster = Array{};

    {
        lue::api::Field result = lue::api::divide(scalar, raster);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = scalar / raster;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(divide_scalar_scalar)
{
    using Element = lue::LargestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar1 = Scalar{};
    lue::api::Field const scalar2 = Scalar{};

    {
        lue::api::Field result = lue::api::divide(scalar1, scalar2);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = scalar1 / scalar2;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(divide_raster_value)
{
    using Element = lue::LargestFloatingPointElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const raster = Array{};
    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::divide(raster, value);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = raster / value;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(divide_value_raster)
{
    using Element = lue::LargestFloatingPointElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const value = Element{};
    lue::api::Field const raster = Array{};

    {
        lue::api::Field result = lue::api::divide(value, raster);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = value / raster;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(divide_scalar_value)
{
    using Element = lue::LargestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar = Scalar{};
    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::divide(scalar, value);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = scalar / value;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(divide_value_scalar)
{
    using Element = lue::LargestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar = Scalar{};
    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::divide(value, scalar);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = value / scalar;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(divide_value_value)
{
    using Element = lue::LargestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const value1 = Element{};
    lue::api::Field const value2 = Element{};

    {
        lue::api::Field result = lue::api::divide(value1, value2);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = value1 / value2;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}
