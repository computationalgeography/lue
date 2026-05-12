#define BOOST_TEST_MODULE lue framework api cxx local valid
#include "lue/framework/api/cxx/local/valid.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(valid_array)
{
    using Element = lue::BooleanElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const array = Array{};

    {
        lue::api::Field result = lue::api::valid(array);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(valid_scalar)
{
    using Element = lue::BooleanElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar = Scalar{};

    {
        lue::api::Field result = lue::api::valid(scalar);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(valid_value)
{
    using Element = lue::BooleanElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::valid(value);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}
