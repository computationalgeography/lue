#define BOOST_TEST_MODULE lue framework api cxx local abs
#include "lue/framework/api/cxx/local/abs.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(abs_array)
{
    using Element = lue::SmallestIntegralElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const array = Array{};

    {
        lue::api::Field result = lue::api::abs(array);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(abs_scalar)
{
    using Element = lue::SmallestIntegralElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar = Scalar{};

    {
        lue::api::Field result = lue::api::abs(scalar);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(abs_value)
{
    using Element = lue::SmallestIntegralElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::abs(value);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}
