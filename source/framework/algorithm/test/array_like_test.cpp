#define BOOST_TEST_MODULE lue framework algorithm array_like
#include "lue/framework/algorithm/all.hpp"
#include "lue/framework/algorithm/array_like.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/definition/equal_to.hpp"
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

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Array input_array{lue::create_partitioned_array<Element>(array_shape, partition_shape)};

        Element const fill_value{5};

        auto output_array = lue::array_like(input_array, fill_value);

        BOOST_CHECK(lue::all(output_array == fill_value).get());
        BOOST_CHECK(lue::all(lue::locality_id(output_array) == lue::locality_id(input_array)).get());
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
