#define BOOST_TEST_MODULE lue framework api cxx field
#include "unit_test.hpp"
#include "lue/framework/api/cxx/field.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(implicit_conversion_from_element)
{
    using Element = lue::FloatingPointElement<0>;
    using lue::api::Field;

    {
        Element const element{};
        Field const field{element};
        BOOST_CHECK(std::holds_alternative<Element>(field.variant()));
    }

    {
        Element const element{};
        Field const field = element;
        BOOST_CHECK(std::holds_alternative<Element>(field.variant()));
    }

    {
        Element element{};
        Field const field{element};
        BOOST_CHECK(std::holds_alternative<Element>(field.variant()));
    }

    {
        Element element{};
        Field const field = element;
        BOOST_CHECK(std::holds_alternative<Element>(field.variant()));
    }
}


BOOST_AUTO_TEST_CASE(implicit_conversion_from_scalar)
{
    using Element = lue::FloatingPointElement<0>;
    using Scalar = lue::Scalar<Element>;
    using lue::api::Field;

    {
        Scalar const scalar{};
        Field const field{scalar};
        BOOST_CHECK(std::holds_alternative<Scalar>(field.variant()));
    }

    {
        Scalar const scalar{};
        Field const field = scalar;
        BOOST_CHECK(std::holds_alternative<Scalar>(field.variant()));
    }

    {
        Scalar scalar{};
        Field const field{scalar};
        BOOST_CHECK(std::holds_alternative<Scalar>(field.variant()));
    }

    {
        Scalar scalar{};
        Field const field = scalar;
        BOOST_CHECK(std::holds_alternative<Scalar>(field.variant()));
    }

    {
        Scalar scalar{};
        Field const field{std::move(scalar)};
        BOOST_CHECK(std::holds_alternative<Scalar>(field.variant()));
    }

    {
        Scalar scalar{};
        Field const field = std::move(scalar);
        BOOST_CHECK(std::holds_alternative<Scalar>(field.variant()));
    }
}


BOOST_AUTO_TEST_CASE(implicit_conversion_from_array)
{
    using Element = lue::FloatingPointElement<0>;
    using Array = lue::PartitionedArray<Element, 2>;
    using lue::api::Field;

    {
        Array array{};
        Field const field{std::move(array)};
        BOOST_CHECK(std::holds_alternative<Array>(field.variant()));
    }

    {
        Array array{};
        Field const field = std::move(array);
        BOOST_CHECK(std::holds_alternative<Array>(field.variant()));
    }
}
