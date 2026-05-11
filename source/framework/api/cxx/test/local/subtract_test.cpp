#define BOOST_TEST_MODULE lue framework api cxx local subtract
#include "lue/framework/api/cxx/operator.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(subtract_raster_raster)
{
    using Element = lue::LargestFloatingPointElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const raster1 = Array{};
    lue::api::Field const raster2 = Array{};

    {
        lue::api::Field result = lue::api::subtract(raster1, raster2);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = raster1 - raster2;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(subtract_raster_scalar)
{
    using Element = lue::LargestFloatingPointElement;
    using Array = lue::PartitionedArray<Element, 2>;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const raster = Array{};
    lue::api::Field const scalar = Scalar{};

    {
        lue::api::Field result = lue::api::subtract(raster, scalar);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = raster - scalar;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(subtract_scalar_raster)
{
    using Element = lue::LargestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const scalar = Scalar{};
    lue::api::Field const raster = Array{};

    {
        lue::api::Field result = lue::api::subtract(scalar, raster);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = scalar - raster;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(subtract_scalar_scalar)
{
    using Element = lue::LargestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar1 = Scalar{};
    lue::api::Field const scalar2 = Scalar{};

    {
        lue::api::Field result = lue::api::subtract(scalar1, scalar2);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = scalar1 - scalar2;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(subtract_raster_value)
{
    using Element = lue::LargestFloatingPointElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const raster = Array{};
    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::subtract(raster, value);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = raster - value;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(subtract_value_raster)
{
    using Element = lue::LargestFloatingPointElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const value = Element{};
    lue::api::Field const raster = Array{};

    {
        lue::api::Field result = lue::api::subtract(value, raster);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = value - raster;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(subtract_scalar_value)
{
    using Element = lue::LargestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar = Scalar{};
    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::subtract(scalar, value);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = scalar - value;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(subtract_value_scalar)
{
    using Element = lue::LargestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar = Scalar{};
    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::subtract(value, scalar);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = value - scalar;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(subtract_value_value)
{
    using Element = lue::LargestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const value1 = Element{};
    lue::api::Field const value2 = Element{};

    {
        lue::api::Field result = lue::api::subtract(value1, value2);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = value1 - value2;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}
