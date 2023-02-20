#define BOOST_TEST_MODULE lue framework algorithm divide
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/divide.hpp"
#include "lue/framework/algorithm/default_policies/equal_to.hpp"
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

        Element const fill_value1{15};
        Element const fill_value2{6};

        Array array1{lue::create_partitioned_array(array_shape, partition_shape, fill_value1)};
        Array array2{lue::create_partitioned_array(array_shape, partition_shape, fill_value2)};

        // Divide two arrays
        {
            auto divide = array1 / array2;

            BOOST_CHECK(all(divide == fill_value1 / fill_value2).get());
        }

        // Divide scalar by array
        // array / scalar
        {
            auto divide = array1 / fill_value1;

            BOOST_CHECK(all(divide == fill_value1 / fill_value1).get());
        }

        // Divide scalar by array
        // scalar / array
        {
            auto divide = fill_value1 / array1;

            BOOST_CHECK(all(divide == fill_value1 / fill_value1).get());
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
// TEST_CASE(2, int32_t)
// TEST_CASE(1, int64_t)
// TEST_CASE(2, int64_t)
// TEST_CASE(1, float)
// TEST_CASE(2, float)
// TEST_CASE(1, double)
TEST_CASE(2, double)

#undef TEST_CASE
