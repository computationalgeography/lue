#define BOOST_TEST_MODULE lue framework api cxx local logical_inclusive_or
#include "lue/framework/api/cxx/operator.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(logical_inclusive_or_array_array)
{
    using Element = lue::BooleanElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const array1 = Array{};
    lue::api::Field const array2 = Array{};

    {
        lue::api::Field result = lue::api::logical_inclusive_or(array1, array2);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = array1 || array2;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(logical_inclusive_or_array_scalar)
{
    using Element = lue::BooleanElement;
    using Array = lue::PartitionedArray<Element, 2>;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const array = Array{};
    lue::api::Field const scalar = Scalar{};

    {
        lue::api::Field result = lue::api::logical_inclusive_or(array, scalar);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = array || scalar;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(logical_inclusive_or_scalar_array)
{
    using Element = lue::BooleanElement;
    using Scalar = lue::Scalar<Element>;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const scalar = Scalar{};
    lue::api::Field const array = Array{};

    {
        lue::api::Field result = lue::api::logical_inclusive_or(scalar, array);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = scalar || array;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(logical_inclusive_or_scalar_scalar)
{
    using Element = lue::BooleanElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar1 = Scalar{};
    lue::api::Field const scalar2 = Scalar{};

    {
        lue::api::Field result = lue::api::logical_inclusive_or(scalar1, scalar2);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = scalar1 || scalar2;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(logical_inclusive_or_array_value)
{
    using Element = lue::BooleanElement;
    using Array = lue::PartitionedArray<Element, 2>;
    using Element = Element;

    lue::api::Field const array = Array{};
    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::logical_inclusive_or(array, value);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = array || value;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(logical_inclusive_or_value_array)
{
    using Element = lue::BooleanElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const value = Element{};
    lue::api::Field const array = Array{};

    {
        lue::api::Field result = lue::api::logical_inclusive_or(value, array);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }

    {
        lue::api::Field result = value || array;

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(logical_inclusive_or_scalar_value)
{
    using Element = lue::BooleanElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar = Scalar{};
    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::logical_inclusive_or(scalar, value);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = scalar || value;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(logical_inclusive_or_value_scalar)
{
    using Element = lue::BooleanElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const scalar = Scalar{};
    lue::api::Field const value = Element{};

    {
        lue::api::Field result = lue::api::logical_inclusive_or(value, scalar);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = value || scalar;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(logical_inclusive_or_value_value)
{
    using Element = lue::BooleanElement;
    using Scalar = lue::Scalar<Element>;

    lue::api::Field const value1 = Element{};
    lue::api::Field const value2 = Element{};

    {
        lue::api::Field result = lue::api::logical_inclusive_or(value1, value2);

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }

    {
        lue::api::Field result = value1 || value2;

        BOOST_CHECK(std::holds_alternative<Scalar>(result.variant()));
    }
}
