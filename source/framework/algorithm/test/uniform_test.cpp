#define BOOST_TEST_MODULE lue framework algorithm uniform
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/all.hpp"
#include "lue/framework/algorithm/equal_to.hpp"
#include "lue/framework/algorithm/greater_equal.hpp"
#include "lue/framework/algorithm/less.hpp"
#include "lue/framework/algorithm/none.hpp"
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

    Array array1{shape};
    Array array2{shape};

    hpx::shared_future<Element> min_value = hpx::make_ready_future<Element>(7);
    hpx::shared_future<Element> max_value = hpx::make_ready_future<Element>(9);

    // Fill two arrays with values from a uniform distribution and check
    // whether
    // - All cells in these arrays are >= min_value
    // - All cells in these arrays are < max_value
    // - All cells in these arrays are different

    lue::uniform_real(array1, min_value, max_value).wait();

    // min_value <= array1 < max_value
    BOOST_CHECK(lue::all(lue::greater_equal(array1, min_value)).get());
    BOOST_CHECK(lue::all(lue::less(array1, max_value)).get());

    lue::uniform_real(array2, min_value, max_value).wait();

    // min_value <= array2 < max_value
    BOOST_CHECK(lue::all(lue::greater_equal(array2, min_value)).get());
    BOOST_CHECK(lue::all(lue::less(array2, max_value)).get());

    // array1 != array2 (or at least very unlikely)
    auto equal_to = lue::equal_to(array1, array2);
    BOOST_CHECK(lue::none(equal_to).get());
}

}  // namespace detail


#define TEST_CASE(                               \
    rank,                                        \
    Element)                                     \
                                                 \
BOOST_AUTO_TEST_CASE(array_##rank##d_##Element,  \
    *boost::unit_test::expected_failures(2))     \
{                                                \
    detail::test_array<Element, rank>();         \
}

TEST_CASE(1, double)
TEST_CASE(2, double)

#undef TEST_CASE
