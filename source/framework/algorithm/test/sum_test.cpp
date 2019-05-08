#define BOOST_TEST_MODULE lue framework algorithm sum
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/sum.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace detail {

template<
    typename Element,
    typename ResultElement,
    std::size_t rank>
void test_array()
{
    using Array = lue::PartitionedArray<Element, rank>;

    auto const shape{lue::Test<Array>::shape()};

    Array array{shape};
    hpx::shared_future<Element> fill_value =
        hpx::make_ready_future<Element>(5);

    // Request the filling of the array and wait for it to finish
    lue::fill(array, fill_value).wait();

    // Request the sumation of the array
    auto sum = lue::sum<Array, ResultElement>(array);

    using TypeWeGot = decltype(sum);
    using TypeWeWant = hpx::future<ResultElement>;
    static_assert(std::is_same_v<TypeWeGot, TypeWeWant>);

    BOOST_CHECK_EQUAL(
        sum.get(),
        static_cast<Element>(lue::nr_elements(shape) * fill_value.get()));
}


template<
    typename Element,
    typename ResultElement>
void test_array_1d()
{
    test_array<Element, ResultElement, 1>();
}


template<
    typename Element,
    typename ResultElement>
void test_array_2d()
{
    test_array<Element, ResultElement, 2>();
}

}  // namespace detail


#define TEST_CASE(                                                 \
    rank,                                                          \
    Element,                                                       \
    ResultElement)                                                 \
                                                                   \
BOOST_AUTO_TEST_CASE(array_##rank##d_##Element##_##ResultElement)  \
{                                                                  \
    detail::test_array_##rank##d<Element, ResultElement>();        \
}

TEST_CASE(1, int32_t, int32_t)
TEST_CASE(2, int32_t, int32_t)
// TEST_CASE(1, int64_t, int64_t)
// TEST_CASE(2, int64_t, int64_t)
// TEST_CASE(1, int32_t, int64_t)
// TEST_CASE(2, int32_t, int64_t)

#undef TEST_CASE
