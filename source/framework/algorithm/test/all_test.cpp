#define BOOST_TEST_MODULE lue framework algorithm all
#include "lue/framework/algorithm/all.hpp"
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

    Element const fill_value1{1};
    Element const fill_value2{0};

    hpx::wait_all(
        lue::fill(array1, fill_value1),
        lue::fill(array2, fill_value2));

    BOOST_CHECK_EQUAL(lue::all(array1).get(), true);
    BOOST_CHECK_EQUAL(lue::all(array2).get(), false);
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

TEST_CASE(1, bool)
TEST_CASE(1, int32_t)
TEST_CASE(2, bool)
TEST_CASE(2, int32_t)

#undef TEST_CASE
