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
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include <limits>


namespace detail {

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

        BOOST_CHECK(all(array_we_got == array_we_want).get());
    }

}  // namespace detail


#define TEST_CASE(rank, InputElement, OutputElement)                                                         \
                                                                                                             \
    BOOST_AUTO_TEST_CASE(array_##rank##d_##InputElement##_##OutputElement)                                   \
    {                                                                                                        \
        detail::test_array<InputElement, OutputElement, rank>();                                             \
    }

// TEST_CASE(2, bool, int32_t)
// TEST_CASE(2, bool, uint32_t)
// TEST_CASE(2, bool, int64_t)
// TEST_CASE(2, bool, uint64_t)
// TEST_CASE(2, bool, float)
// TEST_CASE(2, bool, double)

// TEST_CASE(2, int32_t, int64_t)
// TEST_CASE(2, int32_t, float)
// TEST_CASE(2, int32_t, double)

// TEST_CASE(2, uint32_t, uint64_t)
// TEST_CASE(2, uint32_t, float)
// TEST_CASE(2, uint32_t, double)

TEST_CASE(2, int64_t, float)
TEST_CASE(2, int64_t, double)

TEST_CASE(2, uint64_t, float)
TEST_CASE(2, uint64_t, double)

TEST_CASE(2, float, uint8_t)

#undef TEST_CASE


BOOST_AUTO_TEST_CASE(same_types)
{
    using namespace lue::value_policies;

    using Shape = lue::Shape<lue::Count, 2>;
    Shape const array_shape{3, 2};
    Shape const partition_shape{3, 2};

    // Signed int
    {
        using InputElement = std::int32_t;
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
                min,
                max,
                lowest,
                zero,
                input_no_data,
                data,
            }});
        auto const& array_we_want = array;
        auto const array_we_got = cast<OutputElement>(array);

        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }

    // Unsigned int
    {
        using InputElement = std::uint32_t;
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
                min,
                max,
                lowest,
                zero,
                input_no_data,
                data,
            }});
        auto const& array_we_want = array;
        auto const array_we_got = cast<OutputElement>(array);

        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }

    // Floating point
    {
        using InputElement = float;
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
                min,
                max,
                lowest,
                zero,
                input_no_data,
                data,
            }});
        auto const& array_we_want = array;
        auto const array_we_got = cast<OutputElement>(array);

        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }
}


BOOST_AUTO_TEST_CASE(to_larger_type)
{
    using namespace lue::value_policies;

    using Shape = lue::Shape<lue::Count, 2>;
    Shape const array_shape{3, 2};
    Shape const partition_shape{3, 2};

    // Signed int
    {
        using InputElement = std::int32_t;
        using OutputElement = std::int64_t;

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
                min,
                max,
                lowest,
                zero,
                input_no_data,
                data,
            }});

        OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

        auto const array_we_want =
            lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    min,
                    max,
                    lowest,
                    zero,
                    output_no_data,
                    data,
                }});
        auto const array_we_got = cast<OutputElement>(array);

        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }

    // Unsigned int
    {
        using InputElement = std::uint32_t;
        using OutputElement = std::uint64_t;

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
                min,
                max,
                lowest,
                zero,
                input_no_data,
                data,
            }});

        OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

        auto const array_we_want =
            lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    min,
                    max,
                    lowest,
                    zero,
                    output_no_data,
                    data,
                }});
        auto const array_we_got = cast<OutputElement>(array);

        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }

    // Floating point
    {
        using InputElement = float;
        using OutputElement = double;

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
                min,
                max,
                lowest,
                zero,
                input_no_data,
                data,
            }});

        OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

        auto const array_we_want =
            lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    min,
                    max,
                    lowest,
                    zero,
                    output_no_data,
                    data,
                }});
        auto const array_we_got = cast<OutputElement>(array);

        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }
}


BOOST_AUTO_TEST_CASE(to_smaller_type)
{
    using namespace lue::value_policies;

    using Shape = lue::Shape<lue::Count, 2>;
    Shape const array_shape{3, 2};
    Shape const partition_shape{3, 2};

    // Signed int
    {
        using InputElement = std::int64_t;
        using OutputElement = std::int32_t;

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
                min,
                max,
                lowest,
                input_zero,
                input_no_data,
                data,
            }});

        OutputElement const output_zero{0};
        OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};

        auto const array_we_want =
            lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    output_no_data,
                    output_no_data,
                    output_no_data,
                    output_zero,
                    output_no_data,
                    data,
                }});
        auto const array_we_got = cast<OutputElement>(array);

        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }

    // Unsigned int
    {
        using InputElement = std::uint64_t;
        using OutputElement = std::uint32_t;

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
                input_min,
                max,
                input_lowest,
                input_zero,
                input_no_data,
                data,
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
                    output_min,
                    output_no_data,
                    output_lowest,
                    output_zero,
                    output_no_data,
                    data,
                }});
        auto const array_we_got = cast<OutputElement>(array);

        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }

    // Floating point
    {
        using InputElement = double;
        using OutputElement = float;

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
                min,
                max,
                lowest,
                input_zero,
                input_no_data,
                input_data,
            }});

        OutputElement const output_zero{0};
        OutputElement const output_no_data{lue::policy::no_data_value<OutputElement>};
        OutputElement const output_data{5};

        auto const array_we_want =
            lue::test::create_partitioned_array<lue::PartitionedArray<OutputElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    output_zero,
                    output_no_data,
                    output_no_data,
                    output_zero,
                    output_no_data,
                    output_data,
                }});
        auto const array_we_got = cast<OutputElement>(array);

        lue::test::check_arrays_are_equal(array_we_got, array_we_want);
    }
}


BOOST_AUTO_TEST_CASE(to_different_type)
{
    // TODO
}
