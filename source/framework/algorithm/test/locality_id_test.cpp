#define BOOST_TEST_MODULE lue framework algorithm locality_id
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/locality_id.hpp"
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

    // Request the locality IDs of each partition
    auto locality_id = lue::locality_id(array);

    BOOST_CHECK_EQUAL(locality_id.shape(), array.shape());
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
