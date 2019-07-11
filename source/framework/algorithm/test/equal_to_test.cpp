#define BOOST_TEST_MODULE lue framework algorithm equal_to
#include "lue/framework/algorithm/equal_to.hpp"
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/unique.hpp"
#include "lue/framework/define.hpp"
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

    // Compare two arrays with different values
    {
        auto equal_to = lue::equal_to(array1, array2);
        auto unique = lue::unique(equal_to).get();

        BOOST_REQUIRE_EQUAL(unique.nr_elements(), 1);
        BOOST_CHECK_EQUAL(unique.partitions()[0].data().get()[0], false);
    }

    // Compare two arrays with the same values
    {
        auto equal_to = lue::equal_to(array1, array1);
        auto unique = lue::unique(equal_to).get();

        BOOST_REQUIRE_EQUAL(unique.nr_elements(), 1);
        BOOST_CHECK_EQUAL(unique.partitions()[0].data().get()[0], true);
    }

    // Compare array with scalar
    // array == scalar
    {
        auto equal_to = lue::equal_to(array1, fill_value1);
        auto unique = lue::unique(equal_to).get();

        BOOST_REQUIRE_EQUAL(unique.nr_elements(), 1);
        BOOST_CHECK_EQUAL(unique.partitions()[0].data().get()[0], true);

    }

    // Compare array with scalar
    // scalar == array
    {
        auto equal_to = lue::equal_to(fill_value1, array1);
        auto unique = lue::unique(equal_to).get();

        BOOST_REQUIRE_EQUAL(unique.nr_elements(), 1);
        BOOST_CHECK_EQUAL(unique.partitions()[0].data().get()[0], true);
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
