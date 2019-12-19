#define BOOST_TEST_MODULE lue framework algorithm sqrt
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/all.hpp"
#include "lue/framework/algorithm/equal_to.hpp"
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/sqrt.hpp"
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

    lue::fill(array, hpx::make_ready_future<Element>(25).share()).wait();

    auto sqrt = lue::sqrt(array);

    // FIXME Test by taking a diff from known values and asserting that
    //     the difference is small enough
    BOOST_CHECK(
        lue::all(
            lue::equal_to(
                sqrt,
                hpx::make_ready_future<Element>(5).share())).get()
        );
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

TEST_CASE(1, double)
TEST_CASE(2, double)

#undef TEST_CASE
