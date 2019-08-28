#define BOOST_TEST_MODULE lue framework algorithm copy
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/copy.hpp"
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/sum.hpp"
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

    Array array{shape};
    hpx::shared_future<Element> fill_value =
        hpx::make_ready_future<Element>(5);

    // Request the filling of the array and wait for it to finish
    lue::fill(array, fill_value).wait();

    // Request the copy of the array
    auto copy = lue::copy(array);

    // Request the sumation of the array
    auto sum = lue::sum(copy);

    BOOST_CHECK_EQUAL(
        sum.get(),
        static_cast<Element>(lue::nr_elements(shape) * fill_value.get()));
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
