#define BOOST_TEST_MODULE lue framework api cxx local where
#include "lue/framework/api/cxx/local/where.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(where_array_array)
{
    using ConditionElement = lue::BooleanElement;
    using ConditionArray = lue::PartitionedArray<ConditionElement, 2>;
    using ExpressionElement = lue::LargestSignedIntegralElement;
    using ExpressionArray = lue::PartitionedArray<ExpressionElement, 2>;

    lue::api::Field const condition_array = ConditionArray{};
    lue::api::Field const expression_array = ExpressionArray{};

    {
        lue::api::Field result = lue::api::where(condition_array, expression_array);

        BOOST_CHECK(std::holds_alternative<ExpressionArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(where_array_scalar)
{
    using ConditionElement = lue::BooleanElement;
    using ConditionArray = lue::PartitionedArray<ConditionElement, 2>;
    using ExpressionElement = lue::LargestSignedIntegralElement;
    using ExpressionArray = lue::PartitionedArray<ExpressionElement, 2>;
    using ExpressionScalar = lue::Scalar<ExpressionElement>;

    lue::api::Field const condition_array = ConditionArray{};
    lue::api::Field const expression_scalar = ExpressionScalar{};

    {
        lue::api::Field result = lue::api::where(condition_array, expression_scalar);

        BOOST_CHECK(std::holds_alternative<ExpressionArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(where_array_value)
{
    using ConditionElement = lue::BooleanElement;
    using ConditionArray = lue::PartitionedArray<ConditionElement, 2>;
    using ExpressionElement = lue::LargestSignedIntegralElement;
    using ExpressionArray = lue::PartitionedArray<ExpressionElement, 2>;

    lue::api::Field const condition_array = ConditionArray{};
    lue::api::Field const expression_value = ExpressionElement{};

    {
        lue::api::Field result = lue::api::where(condition_array, expression_value);

        BOOST_CHECK(std::holds_alternative<ExpressionArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(where_array_array_array)
{
    using ConditionElement = lue::BooleanElement;
    using ConditionArray = lue::PartitionedArray<ConditionElement, 2>;
    using ExpressionElement = lue::LargestSignedIntegralElement;
    using ExpressionArray = lue::PartitionedArray<ExpressionElement, 2>;

    lue::api::Field const condition_array = ConditionArray{};
    lue::api::Field const expression_array = ExpressionArray{};

    {
        lue::api::Field result = lue::api::where(condition_array, expression_array, expression_array);

        BOOST_CHECK(std::holds_alternative<ExpressionArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(where_array_array_scalar)
{
    using ConditionElement = lue::BooleanElement;
    using ConditionArray = lue::PartitionedArray<ConditionElement, 2>;
    using ExpressionElement = lue::LargestSignedIntegralElement;
    using ExpressionArray = lue::PartitionedArray<ExpressionElement, 2>;
    using ExpressionScalar = lue::Scalar<ExpressionElement>;

    lue::api::Field const condition_array = ConditionArray{};
    lue::api::Field const expression_array = ExpressionArray{};
    lue::api::Field const expression_scalar = ExpressionScalar{};

    {
        lue::api::Field result = lue::api::where(condition_array, expression_array, expression_scalar);

        BOOST_CHECK(std::holds_alternative<ExpressionArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(where_array_array_value)
{
    using ConditionElement = lue::BooleanElement;
    using ConditionArray = lue::PartitionedArray<ConditionElement, 2>;
    using ExpressionElement = lue::LargestSignedIntegralElement;
    using ExpressionArray = lue::PartitionedArray<ExpressionElement, 2>;

    lue::api::Field const condition_array = ConditionArray{};
    lue::api::Field const expression_array = ExpressionArray{};
    lue::api::Field const expression_value = ExpressionElement{};

    {
        lue::api::Field result = lue::api::where(condition_array, expression_array, expression_value);

        BOOST_CHECK(std::holds_alternative<ExpressionArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(where_array_scalar_array)
{
    using ConditionElement = lue::BooleanElement;
    using ConditionArray = lue::PartitionedArray<ConditionElement, 2>;
    using ExpressionElement = lue::LargestSignedIntegralElement;
    using ExpressionArray = lue::PartitionedArray<ExpressionElement, 2>;
    using ExpressionScalar = lue::Scalar<ExpressionElement>;

    lue::api::Field const condition_array = ConditionArray{};
    lue::api::Field const expression_scalar = ExpressionScalar{};
    lue::api::Field const expression_array = ExpressionArray{};

    {
        lue::api::Field result = lue::api::where(condition_array, expression_scalar, expression_array);

        BOOST_CHECK(std::holds_alternative<ExpressionArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(where_array_value_array)
{
    using ConditionElement = lue::BooleanElement;
    using ConditionArray = lue::PartitionedArray<ConditionElement, 2>;
    using ExpressionElement = lue::LargestSignedIntegralElement;
    using ExpressionArray = lue::PartitionedArray<ExpressionElement, 2>;

    lue::api::Field const condition_array = ConditionArray{};
    lue::api::Field const expression_value = ExpressionElement{};
    lue::api::Field const expression_array = ExpressionArray{};

    {
        lue::api::Field result = lue::api::where(condition_array, expression_value, expression_array);

        BOOST_CHECK(std::holds_alternative<ExpressionArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(where_array_scalar_scalar)
{
    using ConditionElement = lue::BooleanElement;
    using ConditionArray = lue::PartitionedArray<ConditionElement, 2>;
    using ExpressionElement = lue::LargestSignedIntegralElement;
    using ExpressionScalar = lue::Scalar<ExpressionElement>;
    using ExpressionArray = lue::PartitionedArray<ExpressionElement, 2>;

    lue::api::Field const condition_array = ConditionArray{};
    lue::api::Field const expression_scalar = ExpressionScalar{};

    {
        lue::api::Field result = lue::api::where(condition_array, expression_scalar, expression_scalar);

        BOOST_CHECK(std::holds_alternative<ExpressionArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(where_array_scalar_value)
{
    using ConditionElement = lue::BooleanElement;
    using ConditionArray = lue::PartitionedArray<ConditionElement, 2>;
    using ExpressionElement = lue::LargestSignedIntegralElement;
    using ExpressionScalar = lue::Scalar<ExpressionElement>;
    using ExpressionArray = lue::PartitionedArray<ExpressionElement, 2>;

    lue::api::Field const condition_array = ConditionArray{};
    lue::api::Field const expression_scalar = ExpressionScalar{};
    lue::api::Field const expression_value = ExpressionElement{};

    {
        lue::api::Field result = lue::api::where(condition_array, expression_scalar, expression_value);

        BOOST_CHECK(std::holds_alternative<ExpressionArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(where_array_value_scalar)
{
    using ConditionElement = lue::BooleanElement;
    using ConditionArray = lue::PartitionedArray<ConditionElement, 2>;
    using ExpressionElement = lue::LargestSignedIntegralElement;
    using ExpressionScalar = lue::Scalar<ExpressionElement>;
    using ExpressionArray = lue::PartitionedArray<ExpressionElement, 2>;

    lue::api::Field const condition_array = ConditionArray{};
    lue::api::Field const expression_scalar = ExpressionScalar{};
    lue::api::Field const expression_value = ExpressionElement{};

    {
        lue::api::Field result = lue::api::where(condition_array, expression_value, expression_scalar);

        BOOST_CHECK(std::holds_alternative<ExpressionArray>(result.variant()));
    }
}


BOOST_AUTO_TEST_CASE(where_array_value_value)
{
    using ConditionElement = lue::BooleanElement;
    using ConditionArray = lue::PartitionedArray<ConditionElement, 2>;
    using ExpressionElement = lue::LargestSignedIntegralElement;
    using ExpressionArray = lue::PartitionedArray<ExpressionElement, 2>;

    lue::api::Field const condition_array = ConditionArray{};
    lue::api::Field const expression_value = ExpressionElement{};

    {
        lue::api::Field result = lue::api::where(condition_array, expression_value, expression_value);

        BOOST_CHECK(std::holds_alternative<ExpressionArray>(result.variant()));
    }
}
