#define BOOST_TEST_MODULE lue framework algorithm atan2
#include "lue/framework/algorithm/all.hpp"
#include "lue/framework/algorithm/comparison.hpp"
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/trigonometric.hpp"
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

    Element const fill_value1{0.5};
    Element const fill_value2{0.6};

    hpx::wait_all(
        lue::fill(array1, fill_value1),
        lue::fill(array2, fill_value2));

    BOOST_CHECK(
        lue::all(
                lue::atan2(array1, array2) ==
                std::atan2(fill_value1, fill_value2)
            ).get());
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
