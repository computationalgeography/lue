#define BOOST_TEST_MODULE lue framework algorithm greater_than_equal_to
#include "lue/framework/algorithm/all.hpp"
#include "lue/framework/algorithm/comparison.hpp"
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/none.hpp"
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

    Element const fill_value1{5};
    Element const fill_value2{6};

    hpx::wait_all(
        lue::fill(array1, fill_value1),
        lue::fill(array2, fill_value2));

    // Compare two arrays with different values
    {
        BOOST_CHECK(lue::all(array2 >= array1).get());
        BOOST_CHECK(lue::none(array1 >= array2).get());
    }

    // Compare two arrays with the same values
    {
        BOOST_CHECK(lue::all(array2 >= array2).get());
    }

    // Compare array with scalar
    // array >= scalar
    {
        BOOST_CHECK(lue::all(array2 >= fill_value2).get());
        BOOST_CHECK(lue::none(array1 >= fill_value2).get());
    }

    // Compare array with scalar
    // scalar >= array
    {
        BOOST_CHECK(lue::all(fill_value2 >= array2).get());
        BOOST_CHECK(lue::none(fill_value1 >= array2).get());
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
