#define BOOST_TEST_MODULE lue framework algorithm multiply
#include "lue/framework/algorithm/cast.hpp"
#include "lue/framework/algorithm/equal_to.hpp"
#include "lue/framework/algorithm/multiply.hpp"
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/sum.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
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

    hpx::shared_future<Element> fill_value1 =
        hpx::make_ready_future<Element>(5);
    hpx::shared_future<Element> fill_value2 =
        hpx::make_ready_future<Element>(6);

    hpx::wait_all(
        lue::fill(array1, fill_value1),
        lue::fill(array2, fill_value2));



    // Multiply two arrays
    {
        auto multiply = lue::multiply(array1, array2);
        hpx::shared_future<Element> expected_result =
            hpx::make_ready_future<Element>(5 * 6);
        auto equal_to = lue::equal_to(multiply, expected_result);

        auto cast = lue::cast<std::int32_t>(equal_to);
        auto sum = lue::sum(cast);

        BOOST_CHECK_EQUAL(sum.get(), lue::nr_elements(shape));
    }

    // Multiply scalar with array
    // array * scalar
    {
        auto multiply = lue::multiply(array1, fill_value1);
        hpx::shared_future<Element> expected_result =
            hpx::make_ready_future<Element>(5 * 5);
        auto equal_to = lue::equal_to(multiply, expected_result);

        auto cast = lue::cast<std::int32_t>(equal_to);
        auto sum = lue::sum(cast);

        BOOST_CHECK_EQUAL(sum.get(), lue::nr_elements(shape));
    }

    // Multiply scalar with array
    // scalar * array
    {
        auto multiply = lue::multiply(fill_value1, array1);
        hpx::shared_future<Element> expected_result =
            hpx::make_ready_future<Element>(5 * 5);
        auto equal_to = lue::equal_to(multiply, expected_result);

        auto cast = lue::cast<std::int32_t>(equal_to);
        auto sum = lue::sum(cast);

        BOOST_CHECK_EQUAL(sum.get(), lue::nr_elements(shape));
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
