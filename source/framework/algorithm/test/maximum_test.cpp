#define BOOST_TEST_MODULE lue framework algorithm maximum
#include "lue/framework/algorithm/maximum.hpp"
#include "lue/framework/algorithm/unique_id.hpp"
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

    lue::unique_id(array).wait();

    BOOST_CHECK_EQUAL(lue::maximum(array).get(), lue::nr_elements(shape) - 1);
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
TEST_CASE(1, double)
TEST_CASE(2, double)

#undef TEST_CASE
