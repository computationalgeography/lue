#define BOOST_TEST_MODULE lue framework algorithm not_equal_to
#include "lue/framework/algorithm/comparison.hpp"
#include "lue/framework/algorithm/all.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
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

        Element const fill_value1{5};
        Element const fill_value2{6};

        Array array1{lue::create_partitioned_array(array_shape, partition_shape, fill_value1)};
        Array array2{lue::create_partitioned_array(array_shape, partition_shape, fill_value2)};

        // Compare two arrays with different values
        {
            auto not_equal_to = array1 != array2;
            auto all = lue::all(not_equal_to);

            BOOST_CHECK(all.get());
        }

        // Compare two arrays with the same values
        {
            auto not_equal_to = array1 != array1;
            auto none = lue::none(not_equal_to);

            BOOST_CHECK(none.get());
        }

        // Compare array with scalar
        // array != scalar
        {
            auto not_equal_to = array1 != fill_value2;
            auto all = lue::all(not_equal_to);

            BOOST_CHECK(all.get());
        }

        // Compare array with scalar
        // scalar != array
        {
            auto not_equal_to = fill_value2 != array1;
            auto all = lue::all(not_equal_to);

            BOOST_CHECK(all.get());
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

TEST_CASE(1, int32_t)
TEST_CASE(2, int32_t)
// TEST_CASE(1, int64_t)
// TEST_CASE(2, int64_t)
// TEST_CASE(1, float)
// TEST_CASE(2, float)
TEST_CASE(1, double)
TEST_CASE(2, double)

#undef TEST_CASE
