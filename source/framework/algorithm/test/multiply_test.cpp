#define BOOST_TEST_MODULE lue framework algorithm multiply
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/equal_to.hpp"
#include "lue/framework/algorithm/default_policies/multiply.hpp"
#include "lue/framework/algorithm/value_policies/multiply.hpp"
#include "lue/framework/algorithm/value_policies/none.hpp"
#include "lue/framework/algorithm/value_policies/valid.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace detail {

    template<typename Element, std::size_t rank>
    void test_array()
    {
        using namespace lue::default_policies;

        using Array = lue::PartitionedArray<Element, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Element const fill_value1{15};
        Element const fill_value2{6};

        Array array1{lue::create_partitioned_array(array_shape, partition_shape, fill_value1)};
        Array array2{lue::create_partitioned_array(array_shape, partition_shape, fill_value2)};

        // Multiply two arrays
        {
            auto multiply = array1 * array2;
            auto equal_to = multiply == fill_value1 * fill_value2;

            BOOST_CHECK(all(equal_to).future().get());
        }

        // Multiply scalar with array
        // array * scalar
        {
            auto multiply = array1 * fill_value1;
            auto equal_to = multiply == fill_value1 * fill_value1;

            BOOST_CHECK(all(equal_to).future().get());
        }

        // Multiply scalar with array
        // scalar * array
        {
            auto multiply = fill_value1 * array1;
            auto equal_to = multiply == fill_value1 * fill_value1;

            BOOST_CHECK(all(equal_to).future().get());
        }
    }

}  // namespace detail


#define TEST_CASE(rank, Element)                                                                             \
                                                                                                             \
    BOOST_AUTO_TEST_CASE(array_##rank##d_##Element)                                                          \
    {                                                                                                        \
        detail::test_array<Element, rank>();                                                                 \
    }

// TEST_CASE(1, int32_t)
TEST_CASE(2, int32_t)
// TEST_CASE(1, int64_t)
// TEST_CASE(2, int64_t)
// TEST_CASE(1, float)
// TEST_CASE(2, float)
// TEST_CASE(1, double)
TEST_CASE(2, double)

#undef TEST_CASE


BOOST_AUTO_TEST_CASE(out_of_range)
{
    using namespace lue::value_policies;

    using Element = std::int32_t;
    lue::Rank const rank{2};
    using Array = lue::PartitionedArray<Element, rank>;

    auto const array_shape{lue::Test<Array>::shape()};
    auto const partition_shape{lue::Test<Array>::partition_shape()};

    Element const max{std::numeric_limits<Element>::max()};
    Element const fill_value{max};

    Array array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};

    BOOST_CHECK(none(valid<std::uint8_t>(2 * array)).future().get());
    BOOST_CHECK(none(valid<std::uint8_t>(array * 2)).future().get());
    BOOST_CHECK(none(valid<std::uint8_t>(array * array)).future().get());
}
