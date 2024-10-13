#define BOOST_TEST_MODULE lue framework algorithm cast
#include "lue/framework/algorithm/array_like.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/cast.hpp"
#include "lue/framework/algorithm/default_policies/equal_to.hpp"
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include "lue/framework/algorithm/value_policies/all.hpp"
#include "lue/framework/algorithm/value_policies/cast.hpp"
#include "lue/framework/algorithm/value_policies/equal_to.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"
#include <limits>


namespace {

    template<typename InputElement, typename OutputElement, std::size_t rank>
    void test_array()
    {
        using namespace lue::default_policies;

        using InputArray = lue::PartitionedArray<InputElement, rank>;

        auto const array_shape{lue::Test<InputArray>::shape()};
        auto const partition_shape{lue::Test<InputArray>::partition_shape()};

        InputElement const fill_value(5);

        InputArray array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};

        // Request the cast of the array
        auto array_we_got = cast<OutputElement>(array);
        auto array_we_want = lue::array_like(array, static_cast<OutputElement>(fill_value));

        BOOST_CHECK(all(array_we_got == array_we_want).future().get());
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::Rank const rank{2};

    test_array<lue::SignedIntegralElement<0>, lue::FloatingPointElement<0>, rank>();
    test_array<lue::UnsignedIntegralElement<0>, lue::FloatingPointElement<0>, rank>();
    test_array<lue::FloatingPointElement<0>, lue::SignedIntegralElement<0>, rank>();
    test_array<lue::FloatingPointElement<0>, lue::UnsignedIntegralElement<0>, rank>();
}


BOOST_AUTO_TEST_CASE(same_types)
{
    using namespace lue::value_policies;

    using Shape = lue::Shape<lue::Count, 2>;
    Shape const array_shape{3, 2};
    Shape const partition_shape{3, 2};

    // Signed integer
    {
        using InputElement = lue::SignedIntegralElement<0>;
        using OutputElement = InputElement;

        InputElement const min{std::numeric_limits<InputElement>::min() + 1};
        InputElement const max{std::numeric_limits<InputElement>::max()};
        InputElement const lowest{std::numeric_limits<InputElement>::lowest() + 1};
        InputElement const zero{0};
        InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
        InputElement const data{5};

        auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
            array_shape,
            partition_shape,
            {{
                // clang-format off
                min, max,
                lowest, zero,
                input_no_data, data,
                // clang-format on
            }});
        auto const& array_we_want = array;
        auto const array_we_got = cast<OutputElement>(array);
        lue::wait_all(array_we_got.partitions());

        BOOST_TEST_CONTEXT("signed integer")
        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }

    // Unsigned integer
    {
        using InputElement = lue::UnsignedIntegralElement<0>;
        using OutputElement = InputElement;

        InputElement const min{std::numeric_limits<InputElement>::min()};
        InputElement const max{std::numeric_limits<InputElement>::max() - 1};
        InputElement const lowest{std::numeric_limits<InputElement>::lowest()};
        InputElement const zero{0};
        InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
        InputElement const data{5};

        auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
            array_shape,
            partition_shape,
            {{
                // clang-format off
                min, max,
                lowest, zero,
                input_no_data, data,
                // clang-format on
            }});
        auto const& array_we_want = array;
        auto const array_we_got = cast<OutputElement>(array);
        lue::wait_all(array_we_got.partitions());

        BOOST_TEST_CONTEXT("unsigned integer")
        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }

    // Floating point
    {
        using InputElement = lue::FloatingPointElement<0>;
        using OutputElement = InputElement;

        InputElement const min{std::numeric_limits<InputElement>::min()};
        InputElement const max{std::numeric_limits<InputElement>::max()};
        InputElement const lowest{std::numeric_limits<InputElement>::lowest()};
        InputElement const zero{0};
        InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
        InputElement const data{5};

        auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
            array_shape,
            partition_shape,
            {{
                // clang-format off
                min, max,
                lowest, zero,
                input_no_data, data,
                // clang-format on
            }});
        auto const& array_we_want = array;
        auto const array_we_got = cast<OutputElement>(array);
        lue::wait_all(array_we_got.partitions());

        BOOST_TEST_CONTEXT("floating point")
        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }
}


BOOST_AUTO_TEST_CASE(to_larger_type)
{
    using namespace lue::value_policies;

    using Shape = lue::Shape<lue::Count, 2>;
    Shape const array_shape{3, 2};
    Shape const partition_shape{3, 2};

    // Signed integer
    {
        using InputElement = lue::SmallestSignedIntegralElement;
        using OutputElement = lue::LargestSignedIntegralElement;

        if constexpr (!std::is_same_v<InputElement, OutputElement>)
        {
            InputElement const min{std::numeric_limits<InputElement>::min() + 1};
            InputElement const max{std::numeric_limits<InputElement>::max()};
            InputElement const lowest{std::numeric_limits<InputElement>::lowest() + 1};
            InputElement const zero{0};
            InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
            InputElement const data{5};

            auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    min, max,
                    lowest, zero,
                    input_no_data, data,
                    // clang-format on
                }});

            OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

            auto const array_we_want =
                lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                    array_shape,
                    partition_shape,
                    {{
                        // clang-format off
                        min, max,
                        lowest, zero,
                        output_no_data, data,
                        // clang-format on
                    }});
            auto const array_we_got = cast<OutputElement>(array);
            lue::wait_all(array_we_got.partitions());

            BOOST_TEST_CONTEXT("signed integer")
            lue::test::check_arrays_are_equal(array_we_got, array_we_want);
        }
    }

    // Unsigned integer
    {
        using InputElement = lue::SmallestUnsignedIntegralElement;
        using OutputElement = lue::LargestUnsignedIntegralElement;

        if constexpr (!std::is_same_v<InputElement, OutputElement>)
        {
            InputElement const min{std::numeric_limits<InputElement>::min()};
            InputElement const max{std::numeric_limits<InputElement>::max() - 1};
            InputElement const lowest{std::numeric_limits<InputElement>::lowest()};
            InputElement const zero{0};
            InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
            InputElement const data{5};

            auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    min, max,
                    lowest, zero,
                    input_no_data, data,
                    // clang-format on
                }});

            OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

            auto const array_we_want =
                lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                    array_shape,
                    partition_shape,
                    {{
                        // clang-format off
                        min, max,
                        lowest, zero,
                        output_no_data, data,
                        // clang-format on
                    }});
            auto const array_we_got = cast<OutputElement>(array);
            lue::wait_all(array_we_got.partitions());

            BOOST_TEST_CONTEXT("unsigned integer")
            lue::test::check_arrays_are_equal(array_we_got, array_we_want);
        }
    }

    // Floating point
    {
        using InputElement = lue::SmallestFloatingPointElement;
        using OutputElement = lue::LargestFloatingPointElement;

        if constexpr (!std::is_same_v<InputElement, OutputElement>)
        {
            InputElement const min{std::numeric_limits<InputElement>::min()};
            InputElement const max{std::numeric_limits<InputElement>::max()};
            InputElement const lowest{std::numeric_limits<InputElement>::lowest()};
            InputElement const zero{0};
            InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
            InputElement const data{5};

            auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    min, max,
                    lowest, zero,
                    input_no_data, data,
                    // clang-format on
                }});

            OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

            auto const array_we_want =
                lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                    array_shape,
                    partition_shape,
                    {{
                        // clang-format off
                        min, max,
                        lowest, zero,
                        output_no_data, data,
                        // clang-format on
                    }});
            auto const array_we_got = cast<OutputElement>(array);
            lue::wait_all(array_we_got.partitions());

            BOOST_TEST_CONTEXT("floating point")
            lue::test::check_arrays_are_equal(array_we_got, array_we_want);
        }
    }
}


BOOST_AUTO_TEST_CASE(to_smaller_type)
{
    using namespace lue::value_policies;

    using Shape = lue::Shape<lue::Count, 2>;
    Shape const array_shape{3, 2};
    Shape const partition_shape{3, 2};

    // Signed integer
    {
        using InputElement = lue::LargestSignedIntegralElement;
        using OutputElement = lue::SmallestSignedIntegralElement;

        if constexpr (!std::is_same_v<InputElement, OutputElement>)
        {
            InputElement const min{std::numeric_limits<InputElement>::min() + 1};
            InputElement const max{std::numeric_limits<InputElement>::max()};
            InputElement const lowest{std::numeric_limits<InputElement>::lowest() + 1};
            InputElement const input_zero{0};
            InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
            InputElement const data{5};

            auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    min, max,
                    lowest, input_zero,
                    input_no_data, data,
                    // clang-format on
                }});

            OutputElement const output_zero{0};
            OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

            auto const array_we_want =
                lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                    array_shape,
                    partition_shape,
                    {{
                        // clang-format off
                        output_no_data, output_no_data,
                        output_no_data, output_zero,
                        output_no_data, data,
                        // clang-format on
                    }});
            auto const array_we_got = cast<OutputElement>(array);
            lue::wait_all(array_we_got.partitions());

            BOOST_TEST_CONTEXT("signed integer")
            lue::test::check_arrays_are_equal(array_we_got, array_we_want);
        }
    }

    // Unsigned integer
    {
        using InputElement = lue::LargestUnsignedIntegralElement;
        using OutputElement = lue::SmallestUnsignedIntegralElement;

        if constexpr (!std::is_same_v<InputElement, OutputElement>)
        {
            InputElement const input_min{std::numeric_limits<InputElement>::min()};
            InputElement const max{std::numeric_limits<InputElement>::max() - 1};
            InputElement const input_lowest{std::numeric_limits<InputElement>::lowest()};
            InputElement const input_zero{0};
            InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
            InputElement const data{5};

            auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    input_min, max,
                    input_lowest, input_zero,
                    input_no_data, data,
                    // clang-format on
                }});

            OutputElement const output_min{std::numeric_limits<InputElement>::min()};
            OutputElement const output_lowest{std::numeric_limits<InputElement>::lowest()};
            OutputElement const output_zero{0};
            OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

            auto const array_we_want =
                lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                    array_shape,
                    partition_shape,
                    {{
                        // clang-format off
                        output_min, output_no_data,
                        output_lowest, output_zero,
                        output_no_data, data,
                        // clang-format on
                    }});
            auto const array_we_got = cast<OutputElement>(array);
            lue::wait_all(array_we_got.partitions());

            BOOST_TEST_CONTEXT("unsigned integer")
            lue::test::check_arrays_are_equal(array_we_got, array_we_want);
        }
    }

    // Floating point
    {
        using InputElement = lue::LargestFloatingPointElement;
        using OutputElement = lue::SmallestFloatingPointElement;

        if constexpr (!std::is_same_v<InputElement, OutputElement>)
        {
            InputElement const min{std::numeric_limits<InputElement>::min()};
            InputElement const max{std::numeric_limits<InputElement>::max()};
            InputElement const lowest{std::numeric_limits<InputElement>::lowest()};
            InputElement const input_zero{0};
            InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
            InputElement const input_data{5};

            auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    min, max,
                    lowest, input_zero,
                    input_no_data, input_data,
                    // clang-format on
                }});

            OutputElement const output_zero{0};
            OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};
            OutputElement const output_data{5};

            auto const array_we_want =
                lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                    array_shape,
                    partition_shape,
                    {{
                        // clang-format off
                        output_zero, output_no_data,
                        output_no_data, output_zero,
                        output_no_data, output_data,
                        // clang-format on
                    }});
            auto const array_we_got = cast<OutputElement>(array);
            lue::wait_all(array_we_got.partitions());

            BOOST_TEST_CONTEXT("floating point")
            lue::test::check_arrays_are_equal(array_we_got, array_we_want);
        }
    }
}


BOOST_AUTO_TEST_CASE(signed_integer_to_unsigned_integer)
{
    using namespace lue::value_policies;

    using Shape = lue::Shape<lue::Count, 2>;
    Shape const array_shape{3, 2};
    Shape const partition_shape{3, 2};

    // Same sizes
    {
        using InputElement = std::int32_t;
        using OutputElement = std::uint32_t;

        if constexpr (
            lue::arithmetic_element_supported<InputElement> &&
            lue::arithmetic_element_supported<OutputElement>)
        {
            InputElement const min{std::numeric_limits<InputElement>::min() + 1};
            InputElement const max{std::numeric_limits<InputElement>::max()};
            InputElement const lowest{std::numeric_limits<InputElement>::lowest() + 1};
            InputElement const zero{0};
            InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
            InputElement const data{5};

            auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    min, max,
                    lowest, zero,
                    input_no_data, data,
                    // clang-format on
                }});

            OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

            auto const array_we_want =
                lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                    array_shape,
                    partition_shape,
                    {{
                        // clang-format off
                        output_no_data, max,
                        output_no_data, zero,
                        output_no_data, data,
                        // clang-format on
                    }});
            auto const array_we_got = cast<OutputElement>(array);
            lue::wait_all(array_we_got.partitions());

            BOOST_TEST_CONTEXT("same sizes")
            lue::test::check_arrays_are_equal(array_we_got, array_we_want);
        }
    }

    // Small to large
    {
        using InputElement = std::int32_t;
        using OutputElement = std::uint64_t;

        if constexpr (
            lue::arithmetic_element_supported<InputElement> &&
            lue::arithmetic_element_supported<OutputElement>)
        {
            InputElement const min{std::numeric_limits<InputElement>::min() + 1};
            InputElement const max{std::numeric_limits<InputElement>::max()};
            InputElement const lowest{std::numeric_limits<InputElement>::lowest() + 1};
            InputElement const zero{0};
            InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
            InputElement const data{5};

            auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    min, max,
                    lowest, zero,
                    input_no_data, data,
                    // clang-format on
                }});

            OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

            auto const array_we_want =
                lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                    array_shape,
                    partition_shape,
                    {{
                        // clang-format off
                        output_no_data, max,
                        output_no_data, zero,
                        output_no_data, data,
                        // clang-format on
                    }});
            auto const array_we_got = cast<OutputElement>(array);
            lue::wait_all(array_we_got.partitions());

            BOOST_TEST_CONTEXT("small to large")
            lue::test::check_arrays_are_equal(array_we_got, array_we_want);
        }
    }

    // Large to small
    {
        using InputElement = std::int64_t;
        using OutputElement = std::uint32_t;

        if constexpr (
            lue::arithmetic_element_supported<InputElement> &&
            lue::arithmetic_element_supported<OutputElement>)
        {
            InputElement const min{std::numeric_limits<InputElement>::min() + 1};
            InputElement const input_max{std::numeric_limits<InputElement>::max()};
            InputElement const lowest{std::numeric_limits<InputElement>::lowest() + 1};
            InputElement const zero{0};
            InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
            InputElement const data{5};

            auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    min, input_max,
                    lowest, zero,
                    input_no_data, data,
                    // clang-format on
                }});

            OutputElement const output_max{std::numeric_limits<OutputElement>::max()};
            OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

            auto const array_we_want =
                lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                    array_shape,
                    partition_shape,
                    {{
                        // clang-format off
                        output_no_data, output_max,
                        output_no_data, zero,
                        output_no_data, data,
                        // clang-format on
                    }});
            auto const array_we_got = cast<OutputElement>(array);
            lue::wait_all(array_we_got.partitions());

            BOOST_TEST_CONTEXT("large to small")
            lue::test::check_arrays_are_equal(array_we_got, array_we_want);
        }
    }
}


BOOST_AUTO_TEST_CASE(unsigned_integer_to_signed_integer)
{
    using namespace lue::value_policies;

    using Shape = lue::Shape<lue::Count, 2>;
    Shape const array_shape{3, 2};
    Shape const partition_shape{3, 2};

    // Same sizes
    {
        using InputElement = std::uint32_t;
        using OutputElement = std::int32_t;

        if constexpr (
            lue::arithmetic_element_supported<InputElement> &&
            lue::arithmetic_element_supported<OutputElement>)
        {
            InputElement const min{std::numeric_limits<InputElement>::min()};
            InputElement const max{std::numeric_limits<InputElement>::max() - 1};
            InputElement const lowest{std::numeric_limits<InputElement>::lowest()};
            InputElement const zero{0};
            InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
            InputElement const data{5};

            auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    min, max,
                    lowest, zero,
                    input_no_data, data,
                    // clang-format on
                }});

            OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

            auto const array_we_want =
                lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                    array_shape,
                    partition_shape,
                    {{
                        // clang-format off
                        min, output_no_data,
                        lowest, zero,
                        output_no_data, data,
                        // clang-format on
                    }});
            auto const array_we_got = cast<OutputElement>(array);
            lue::wait_all(array_we_got.partitions());

            BOOST_TEST_CONTEXT("same sizes")
            lue::test::check_arrays_are_equal(array_we_got, array_we_want);
        }
    }

    // Small to large
    {
        using InputElement = std::uint32_t;
        using OutputElement = std::int64_t;

        if constexpr (
            lue::arithmetic_element_supported<InputElement> &&
            lue::arithmetic_element_supported<OutputElement>)
        {
            InputElement const min{std::numeric_limits<InputElement>::min()};
            InputElement const max{std::numeric_limits<InputElement>::max() - 1};
            InputElement const lowest{std::numeric_limits<InputElement>::lowest()};
            InputElement const zero{0};
            InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
            InputElement const data{5};

            auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    min, max,
                    lowest, zero,
                    input_no_data, data,
                    // clang-format on
                }});

            OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

            auto const array_we_want =
                lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                    array_shape,
                    partition_shape,
                    {{
                        // clang-format off
                        min, max,
                        lowest, zero,
                        output_no_data, data,
                        // clang-format on
                    }});
            auto const array_we_got = cast<OutputElement>(array);
            lue::wait_all(array_we_got.partitions());

            BOOST_TEST_CONTEXT("small to large")
            lue::test::check_arrays_are_equal(array_we_got, array_we_want);
        }
    }

    // Large to small
    {
        using InputElement = std::uint64_t;
        using OutputElement = std::int32_t;

        if constexpr (
            lue::arithmetic_element_supported<InputElement> &&
            lue::arithmetic_element_supported<OutputElement>)
        {
            InputElement const min{std::numeric_limits<InputElement>::min()};
            InputElement const max{std::numeric_limits<InputElement>::max() - 1};
            InputElement const lowest{std::numeric_limits<InputElement>::lowest()};
            InputElement const zero{0};
            InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
            InputElement const data{5};

            auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    min, max,
                    lowest, zero,
                    input_no_data, data,
                    // clang-format on
                }});

            OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

            auto const array_we_want =
                lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                    array_shape,
                    partition_shape,
                    {{
                        // clang-format off
                        min, output_no_data,
                        lowest, zero,
                        output_no_data, data,
                        // clang-format on
                    }});
            auto const array_we_got = cast<OutputElement>(array);
            lue::wait_all(array_we_got.partitions());

            BOOST_TEST_CONTEXT("large to small")
            lue::test::check_arrays_are_equal(array_we_got, array_we_want);
        }
    }
}


BOOST_AUTO_TEST_CASE(signed_integer_to_floating_point)
{
    using namespace lue::value_policies;

    using Shape = lue::Shape<lue::Count, 2>;
    Shape const array_shape{3, 2};
    Shape const partition_shape{3, 2};

    {
        using InputElement = lue::LargestSignedIntegralElement;
        using OutputElement = lue::SmallestFloatingPointElement;

        InputElement const input_min{std::numeric_limits<InputElement>::min() + 1};
        InputElement const input_max{std::numeric_limits<InputElement>::max()};
        InputElement const input_lowest{std::numeric_limits<InputElement>::lowest() + 1};
        InputElement const zero{0};
        InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
        InputElement const data{5};

        auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
            array_shape,
            partition_shape,
            {{
                // clang-format off
                input_min, input_max,
                input_lowest, zero,
                input_no_data, data,
                // clang-format on
            }});

        OutputElement const output_min{static_cast<OutputElement>(input_min)};
        OutputElement const output_max{static_cast<OutputElement>(input_max)};
        OutputElement const output_lowest{static_cast<OutputElement>(input_lowest)};
        OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

        auto const array_we_want =
            lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    output_min, output_max,
                    output_lowest, zero,
                    output_no_data, data,
                    // clang-format on
                }});
        auto const array_we_got = cast<OutputElement>(array);
        lue::wait_all(array_we_got.partitions());

        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }
}


BOOST_AUTO_TEST_CASE(unsigned_integer_to_floating_point)
{
    using namespace lue::value_policies;

    using Shape = lue::Shape<lue::Count, 2>;
    Shape const array_shape{3, 2};
    Shape const partition_shape{3, 2};

    {
        using InputElement = lue::LargestUnsignedIntegralElement;
        using OutputElement = lue::SmallestFloatingPointElement;

        InputElement const min{std::numeric_limits<InputElement>::min()};
        InputElement const input_max{std::numeric_limits<InputElement>::max() - 1};
        InputElement const lowest{std::numeric_limits<InputElement>::lowest()};
        InputElement const zero{0};
        InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
        InputElement const data{5};

        auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
            array_shape,
            partition_shape,
            {{
                // clang-format off
                min, input_max,
                lowest, zero,
                input_no_data, data,
                // clang-format on
            }});

        OutputElement const output_max{static_cast<OutputElement>(input_max)};
        OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

        auto const array_we_want =
            lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    min, output_max,
                    lowest, zero,
                    output_no_data, data,
                    // clang-format on
                }});
        auto const array_we_got = cast<OutputElement>(array);
        lue::wait_all(array_we_got.partitions());

        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }
}


BOOST_AUTO_TEST_CASE(floating_point_to_signed_integer)
{
    using namespace lue::value_policies;

    using Shape = lue::Shape<lue::Count, 2>;
    Shape const array_shape{3, 2};
    Shape const partition_shape{3, 2};

    {
        using InputElement = lue::SmallestFloatingPointElement;
        using OutputElement = lue::LargestSignedIntegralElement;

        InputElement const input_min{std::numeric_limits<InputElement>::min()};
        InputElement const input_max{std::numeric_limits<InputElement>::max()};
        InputElement const input_lowest{std::numeric_limits<InputElement>::lowest()};
        InputElement const input_zero{0};
        InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
        InputElement const input_data{5};

        auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
            array_shape,
            partition_shape,
            {{
                // clang-format off
                input_min, input_max,
                input_lowest, input_zero,
                input_no_data, input_data,
                // clang-format on
            }});

        OutputElement const output_zero{0};
        OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};
        OutputElement const output_data{5};

        auto const array_we_want =
            lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    output_zero, output_no_data,
                    output_no_data, output_zero,
                    output_no_data, output_data,
                    // clang-format on
                }});
        auto const array_we_got = cast<OutputElement>(array);
        lue::wait_all(array_we_got.partitions());

        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }
}


BOOST_AUTO_TEST_CASE(floating_point_to_unsigned_integer)
{
    using namespace lue::value_policies;

    using Shape = lue::Shape<lue::Count, 2>;
    Shape const array_shape{3, 2};
    Shape const partition_shape{3, 2};

    {
        using InputElement = lue::SmallestFloatingPointElement;
        using OutputElement = lue::LargestUnsignedIntegralElement;

        InputElement const input_min{std::numeric_limits<InputElement>::min()};
        InputElement const input_max{std::numeric_limits<InputElement>::max()};
        InputElement const input_lowest{std::numeric_limits<InputElement>::lowest()};
        InputElement const input_zero{0};
        InputElement const input_no_data{lue::policy::no_data_value<InputElement>};
        InputElement const input_data{5};

        auto const array = lue::test::create_partitioned_array<lue::PartitionedArray<InputElement, 2>>(
            array_shape,
            partition_shape,
            {{
                // clang-format off
                input_min, input_max,
                input_lowest, input_zero,
                input_no_data, input_data,
                // clang-format on
            }});

        OutputElement const output_zero{0};
        OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};
        OutputElement const output_data{5};

        auto const array_we_want =
            lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    // clang-format off
                    output_zero, output_no_data,
                    output_no_data, output_zero,
                    output_no_data, output_data,
                    // clang-format on
                }});
        auto const array_we_got = cast<OutputElement>(array);
        lue::wait_all(array_we_got.partitions());

        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }
}
