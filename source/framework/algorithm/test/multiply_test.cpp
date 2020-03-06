#define BOOST_TEST_MODULE lue framework algorithm multiply
#include "lue/framework/algorithm/all.hpp"
#include "lue/framework/algorithm/arithmetic.hpp"
#include "lue/framework/algorithm/comparison.hpp"
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace detail {

template<
    typename Element,
    std::size_t rank>
void test_array()
{
    using Array = lue::PartitionedArray<Element, rank>;

    auto const shape{lue::Test<Array>::shape()};

    Array array1{shape};
    Array array2{shape};

    Element const fill_value1{15};
    Element const fill_value2{6};

    hpx::wait_all(
        lue::fill(array1, fill_value1),
        lue::fill(array2, fill_value2));

    // Multiply two arrays
    {
        auto multiply = array1 * array2;
        auto equal_to = multiply == fill_value1 * fill_value2;

        BOOST_CHECK(lue::all(equal_to).get());
    }

    // Multiply scalar with array
    // array * scalar
    {
        auto multiply = array1 * fill_value1;
        auto equal_to = multiply == fill_value1 * fill_value1;

        BOOST_CHECK(lue::all(equal_to).get());
    }

    // Multiply scalar with array
    // scalar * array
    {
        auto multiply = fill_value1 * array1;
        auto equal_to = multiply == fill_value1 * fill_value1;

        BOOST_CHECK(lue::all(equal_to).get());
    }
}

}  // namespace detail


#define TEST_CASE(                               \
    rank,                                        \
    Element)                                     \
                                                 \
BOOST_AUTO_TEST_CASE(array_##rank##d_##Element)  \
{                                                \
    detail::test_array<Element, rank>();         \
}

TEST_CASE(1, int32_t)
TEST_CASE(2, int32_t)
// TEST_CASE(1, int64_t)
// TEST_CASE(2, int64_t)
// TEST_CASE(1, float)
// TEST_CASE(2, float)
TEST_CASE(1, double)
TEST_CASE(2, double)

#undef TEST_CASE
