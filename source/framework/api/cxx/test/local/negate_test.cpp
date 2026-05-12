#define BOOST_TEST_MODULE lue framework api cxx local negate
#include "lue/framework/api/cxx/operator.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(negate_array)
{
    using Element = lue::SmallestIntegralElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const array = Array{};

    {
        lue::api::Field result = lue::api::negate(array);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = -array;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(negate_scalar)
{
    using Element = lue::SmallestIntegralElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar = Scalar{};

    {
        lue::api::Field result = lue::api::negate(scalar);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = -scalar;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(negate_value)
{
    using Element = lue::SmallestIntegralElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::negate(value);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = -value;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}
