#define BOOST_TEST_MODULE lue framework algorithm close_to
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/close_to.hpp"
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

        Element const fill_value1{5};
        Element const fill_value2{6};

        Array array1{lue::create_partitioned_array(array_shape, partition_shape, fill_value1)};
        Array array2{lue::create_partitioned_array(array_shape, partition_shape, fill_value2)};

        // Compare two arrays with different values
        {
            BOOST_CHECK(none(close_to(array1, array2)).get());
        }

        // Compare two arrays with the same values
        {
            BOOST_CHECK(all(close_to(array1, array1)).get());
        }

        // Compare array with scalar
        // array == scalar
        {
            BOOST_CHECK(all(close_to(array1, fill_value1)).get());
        }

        // Compare array with scalar
        // scalar == array
        {
            BOOST_CHECK(all(close_to(fill_value1, array1)).get());
        }

        // TODO Add tests with close values
    }

}  // namespace detail


#define TEST_CASE(rank, Element)                                                                             \
                                                                                                             \
    BOOST_AUTO_TEST_CASE(array_##rank##d_##Element)                                                          \
    {                                                                                                        \
        detail::test_array<Element, rank>();                                                                 \
    }

// TEST_CASE(1, float)
// TEST_CASE(2, float)
// TEST_CASE(1, double)
TEST_CASE(2, double)

#undef TEST_CASE
