#define BOOST_TEST_MODULE lue framework api cxx local greater_than
#include "lue/framework/api/cxx/operator.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(greater_than_array_array)
{
    using InputElement = lue::LargestSignedIntegralElement;
    using InputArray = lue::PartitionedArray<InputElement, 2>;
    using OutputElement = lue::BooleanElement;
    using OutputArray = lue::PartitionedArray<OutputElement, 2>;

    lue::api::Field const array1 = InputArray{};
    lue::api::Field const array2 = InputArray{};

    {
        lue::api::Field result = lue::api::greater_than(array1, array2);

        BOOST_CHECK(std::holds_alternative<OutputArray>(result.variant()));
    }

    {
        lue::api::Field result = array1 > array2;

        BOOST_CHECK(std::holds_alternative<OutputArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(greater_than_array_scalar)
{
    using InputElement = lue::LargestSignedIntegralElement;
    using InputArray = lue::PartitionedArray<InputElement, 2>;
    using InputScalar = lue::Scalar<InputElement>;
    using OutputElement = lue::BooleanElement;
    using OutputArray = lue::PartitionedArray<OutputElement, 2>;

    lue::api::Field const array = InputArray{};
    lue::api::Field const scalar = InputScalar{};

    {
        lue::api::Field result = lue::api::greater_than(array, scalar);

        BOOST_CHECK(std::holds_alternative<OutputArray>(result.variant()));
    }

    {
        lue::api::Field result = array > scalar;

        BOOST_CHECK(std::holds_alternative<OutputArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(greater_than_scalar_array)
{
    using InputElement = lue::LargestSignedIntegralElement;
    using InputScalar = lue::Scalar<InputElement>;
    using InputArray = lue::PartitionedArray<InputElement, 2>;
    using OutputElement = lue::BooleanElement;
    using OutputArray = lue::PartitionedArray<OutputElement, 2>;

    lue::api::Field const scalar = InputScalar{};
    lue::api::Field const array = InputArray{};

    {
        lue::api::Field result = lue::api::greater_than(scalar, array);

        BOOST_CHECK(std::holds_alternative<OutputArray>(result.variant()));
    }

    {
        lue::api::Field result = scalar > array;

        BOOST_CHECK(std::holds_alternative<OutputArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(greater_than_scalar_scalar)
{
    using InputElement = lue::LargestSignedIntegralElement;
    using InputScalar = lue::Scalar<InputElement>;
    using OutputElement = lue::BooleanElement;
    using OutputScalar = lue::Scalar<OutputElement>;

    lue::api::Field const scalar1 = InputScalar{};
    lue::api::Field const scalar2 = InputScalar{};

    {
        lue::api::Field result = lue::api::greater_than(scalar1, scalar2);

        BOOST_CHECK(std::holds_alternative<OutputScalar>(result.variant()));
    }

    {
        lue::api::Field result = scalar1 > scalar2;

        BOOST_CHECK(std::holds_alternative<OutputScalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(greater_than_array_value)
{
    using InputElement = lue::LargestSignedIntegralElement;
    using InputArray = lue::PartitionedArray<InputElement, 2>;
    using OutputElement = lue::BooleanElement;
    using OutputArray = lue::PartitionedArray<OutputElement, 2>;

    lue::api::Field const array = InputArray{};
    lue::api::Field const value = InputElement{};

    {
        lue::api::Field result = lue::api::greater_than(array, value);

        BOOST_CHECK(std::holds_alternative<OutputArray>(result.variant()));
    }

    {
        lue::api::Field result = array > value;

        BOOST_CHECK(std::holds_alternative<OutputArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(greater_than_value_array)
{
    using InputElement = lue::LargestSignedIntegralElement;
    using InputArray = lue::PartitionedArray<InputElement, 2>;
    using OutputElement = lue::BooleanElement;
    using OutputArray = lue::PartitionedArray<OutputElement, 2>;

    lue::api::Field const value = InputElement{};
    lue::api::Field const array = InputArray{};

    {
        lue::api::Field result = lue::api::greater_than(value, array);

        BOOST_CHECK(std::holds_alternative<OutputArray>(result.variant()));
    }

    {
        lue::api::Field result = value > array;

        BOOST_CHECK(std::holds_alternative<OutputArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(greater_than_scalar_value)
{
    using InputElement = lue::LargestSignedIntegralElement;
    using InputScalar = lue::Scalar<InputElement>;
    using OutputElement = lue::BooleanElement;
    using OutputScalar = lue::Scalar<OutputElement>;

    lue::api::Field const scalar = InputScalar{};
    lue::api::Field const value = InputElement{};

    {
        lue::api::Field result = lue::api::greater_than(scalar, value);

        BOOST_CHECK(std::holds_alternative<OutputScalar>(result.variant()));
    }

    {
        lue::api::Field result = scalar > value;

        BOOST_CHECK(std::holds_alternative<OutputScalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(greater_than_value_scalar)
{
    using InputElement = lue::LargestSignedIntegralElement;
    using InputScalar = lue::Scalar<InputElement>;
    using OutputElement = lue::BooleanElement;
    using OutputScalar = lue::Scalar<OutputElement>;

    lue::api::Field const scalar = InputScalar{};
    lue::api::Field const value = InputElement{};

    {
        lue::api::Field result = lue::api::greater_than(value, scalar);

        BOOST_CHECK(std::holds_alternative<OutputScalar>(result.variant()));
    }

    {
        lue::api::Field result = value > scalar;

        BOOST_CHECK(std::holds_alternative<OutputScalar>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(greater_than_value_value)
{
    using InputElement = lue::LargestSignedIntegralElement;
    using OutputElement = lue::BooleanElement;
    using OutputScalar = lue::Scalar<OutputElement>;

    lue::api::Field const value1 = InputElement{};
    lue::api::Field const value2 = InputElement{};

    {
        lue::api::Field result = lue::api::greater_than(value1, value2);

        BOOST_CHECK(std::holds_alternative<OutputScalar>(result.variant()));
    }

    {
        lue::api::Field result = value1 > value2;

        BOOST_CHECK(std::holds_alternative<OutputScalar>(result.variant()));
    }
}
