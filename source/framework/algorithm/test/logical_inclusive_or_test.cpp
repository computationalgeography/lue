#define BOOST_TEST_MODULE lue framework algorithm logical_inclusive_or
#include "lue/framework/algorithm/all.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/definition/logical_inclusive_or.hpp"
#include "lue/framework/algorithm/none.hpp"
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

        Element const fill_value1{1};  // true
        Element const fill_value2{0};  // false

        Array array1{lue::create_partitioned_array(array_shape, partition_shape, fill_value1)};
        Array array2{lue::create_partitioned_array(array_shape, partition_shape, fill_value2)};

        // array || array
        {
            BOOST_CHECK(lue::all(array1 || array2).get());  // true || false
            BOOST_CHECK(lue::all(array1 || array1).get());  // true || true
            BOOST_CHECK(lue::none(array2 || array2).get());  // false || false
        }

        // array || scalar
        {
            BOOST_CHECK(lue::all(array1 || fill_value1).get());
            BOOST_CHECK(lue::all(array1 || fill_value2).get());
        }

        // scalar || array
        {
            BOOST_CHECK(lue::all(fill_value1 || array1).get());
            BOOST_CHECK(lue::all(fill_value2 || array1).get());
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

TEST_CASE(1, uint8_t)
TEST_CASE(2, uint8_t)

#undef TEST_CASE
