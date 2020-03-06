#define BOOST_TEST_MODULE lue framework algorithm sin
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

    Array array{shape};
    Element const fill_value{0.5};

    lue::fill(array, fill_value).wait();

    BOOST_CHECK(lue::all(lue::sin(array) == std::sin(fill_value)).get());
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
