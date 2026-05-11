#define BOOST_TEST_MODULE lue framework api cxx local pow
#include "lue/framework/api/cxx/local/pow.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(pow_raster_raster)
{
    using Element = lue::LargestFloatingPointElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const raster1 = Array{};
    lue::api::Field const raster2 = Array{};

    {
        lue::api::Field result = lue::api::pow(raster1, raster2);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(pow_raster_scalar)
{
    using Element = lue::LargestFloatingPointElement;
    using Array = lue::PartitionedArray<Element, 2>;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const raster = Array{};
    lue::api::Field const scalar = Scalar{};

    {
        lue::api::Field result = lue::api::pow(raster, scalar);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(pow_scalar_raster)
{
    using Element = lue::LargestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const scalar = Scalar{};
    lue::api::Field const raster = Array{};

    {
        lue::api::Field result = lue::api::pow(scalar, raster);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(pow_scalar_scalar)
{
    using Element = lue::LargestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar1 = Scalar{};
    lue::api::Field const scalar2 = Scalar{};

    {
        lue::api::Field result = lue::api::pow(scalar1, scalar2);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(pow_raster_value)
{
    using Element = lue::LargestFloatingPointElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const raster = Array{};
    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::pow(raster, value);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(pow_value_raster)
{
    using Element = lue::LargestFloatingPointElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const value = Element{};
    lue::api::Field const raster = Array{};

    {
        lue::api::Field result = lue::api::pow(value, raster);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(pow_scalar_value)
{
    using Element = lue::LargestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar = Scalar{};
    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::pow(scalar, value);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(pow_value_scalar)
{
    using Element = lue::LargestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar = Scalar{};
    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::pow(value, scalar);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(pow_value_value)
{
    using Element = lue::LargestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const value1 = Element{};
    lue::api::Field const value2 = Element{};

    {
        lue::api::Field result = lue::api::pow(value1, value2);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}
