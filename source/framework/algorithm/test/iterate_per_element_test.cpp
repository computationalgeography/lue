#define BOOST_TEST_MODULE lue framework algorithm iterate_per_element
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/all.hpp"
#include "lue/framework/algorithm/equal_to.hpp"
#include "lue/framework/algorithm/iterate_per_element.hpp"
#include "lue/framework/algorithm/uniform.hpp"
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
    hpx::shared_future<Element> min_nr_iterations =
        hpx::make_ready_future<Element>(100);
    hpx::shared_future<Element> max_nr_iterations =
        hpx::make_ready_future<Element>(500);

    // Request the filling of the array and wait for it to finish
    array = lue::uniform(array, min_nr_iterations, max_nr_iterations);

    auto copy = lue::iterate_per_element(array);

    BOOST_CHECK(lue::all(lue::equal_to(copy, array)).get());
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

#undef TEST_CASE
