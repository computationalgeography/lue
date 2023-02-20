#define BOOST_TEST_MODULE lue framework algorithm greater_than
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/greater_than.hpp"
#include "lue/framework/algorithm/default_policies/none.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace detail {

    template<typename Element, std::size_t rank>
    void test_array()
    {
        using namespace lue::default_policies;

        using Array = lue::PartitionedArray<Element, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Element const fill_value1{5};
        Element const fill_value2{6};

        Array array1{lue::create_partitioned_array(array_shape, partition_shape, fill_value1)};
        Array array2{lue::create_partitioned_array(array_shape, partition_shape, fill_value2)};

        // Compare two arrays with different values
        {
            BOOST_CHECK(none(array1 > array2).get());
            BOOST_CHECK(all(array2 > array1).get());
        }

        // Compare two arrays with the same values
        {
            BOOST_CHECK(none(array2 > array2).get());
        }

        // Compare array with scalar
        // array < scalar
        {
            BOOST_CHECK(none(array2 > fill_value2).get());
            BOOST_CHECK(all(array2 > fill_value1).get());
        }

        // Compare array with scalar
        // scalar < array
        {
            BOOST_CHECK(none(fill_value2 > array2).get());
            BOOST_CHECK(all(fill_value2 > array1).get());
        }
    }

}  // namespace detail


#define TEST_CASE(rank, Element)                                                                             \
                                                                                                             \
    BOOST_AUTO_TEST_CASE(array_##rank##d_##Element)                                                          \
    {                                                                                                        \
        detail::test_array<Element, rank>();                                                                 \
    }

// TEST_CASE(1, int32_t)
TEST_CASE(2, int32_t)
// TEST_CASE(1, int64_t)
// TEST_CASE(2, int64_t)
// TEST_CASE(1, float)
// TEST_CASE(2, float)
// TEST_CASE(1, double)
TEST_CASE(2, double)

#undef TEST_CASE
