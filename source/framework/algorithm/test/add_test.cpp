#define BOOST_TEST_MODULE lue framework algorithm add
#include "lue/framework/algorithm/add.hpp"
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

    auto add = lue::add(array1, array2);
    auto sum = lue::sum(add);

    BOOST_CHECK_EQUAL(
        sum.get(),
        static_cast<Element>(
            lue::nr_elements(shape) *
            (fill_value1.get() + fill_value2.get())));
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
