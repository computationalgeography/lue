#define BOOST_TEST_MODULE lue framework algorithm maximum
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/maximum.hpp"
#include "lue/framework/algorithm/range.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace detail {

    template<
        typename Element,
        std::size_t rank>
    void test_array()
    {
        using namespace lue::default_policies;

        using Array = lue::PartitionedArray<Element, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Array array{lue::create_partitioned_array<Element>(array_shape, partition_shape)};

        lue::range(array, Element{0}).get();

        BOOST_CHECK_EQUAL(maximum(array).get(), lue::nr_elements(array_shape) - 1);
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


// TEST_CASE(1, int32_t)
TEST_CASE(2, int32_t)
// TEST_CASE(1, double)
TEST_CASE(2, double)

#undef TEST_CASE
