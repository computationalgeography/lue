#define BOOST_TEST_MODULE lue framework algorithm logical_exclusive_or
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/logical_exclusive_or.hpp"
#include "lue/framework/algorithm/default_policies/none.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace detail {

    template<typename Element, std::size_t rank>
    void test_array()
    {
        using namespace lue::default_policies;

        using Array = lue::PartitionedArray<Element, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Element const fill_value1{1};  // true
        Element const fill_value2{0};  // false

        Array array1{lue::create_partitioned_array(array_shape, partition_shape, fill_value1)};
        Array array2{lue::create_partitioned_array(array_shape, partition_shape, fill_value2)};

        // array || array
        {
            BOOST_CHECK(all(logical_exclusive_or(array1, array2)).get());   // true || false
            BOOST_CHECK(none(logical_exclusive_or(array1, array1)).get());  // true || true
            BOOST_CHECK(none(logical_exclusive_or(array2, array2)).get());  // false || false
        }

        // array || scalar
        {
            BOOST_CHECK(none(logical_exclusive_or(array1, fill_value1)).get());
            BOOST_CHECK(all(logical_exclusive_or(array1, fill_value2)).get());
        }

        // scalar || array
        {
            BOOST_CHECK(none(logical_exclusive_or(fill_value1, array1)).get());
            BOOST_CHECK(all(logical_exclusive_or(fill_value2, array1)).get());
        }
    }

}  // namespace detail


#define TEST_CASE(rank, Element)                                                                             \
                                                                                                             \
    BOOST_AUTO_TEST_CASE(array_##rank##d_##Element)                                                          \
    {                                                                                                        \
        detail::test_array<Element, rank>();                                                                 \
    }

// TEST_CASE(1, uint8_t)
TEST_CASE(2, uint8_t)

#undef TEST_CASE
