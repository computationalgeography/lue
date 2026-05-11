#define BOOST_TEST_MODULE lue framework api cxx local log10
#include "lue/framework/api/cxx/local/log10.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(log10_array)
{
    using Element = lue::SmallestFloatingPointElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const array = Array{};

    {
        lue::api::Field result = lue::api::log10(array);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(log10_scalar)
{
    using Element = lue::SmallestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar = Scalar{};

    {
        lue::api::Field result = lue::api::log10(scalar);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(log10_value)
{
    using Element = lue::SmallestFloatingPointElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::log10(value);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}
