#define BOOST_TEST_MODULE lue framework algorithm unique
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/unique.hpp"
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

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        // All the same values
        {
            Element const fill_value{5};
            Array array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};

            auto unique = lue::unique(array).get();

            BOOST_REQUIRE_EQUAL(unique.nr_elements(), 1);
            BOOST_CHECK_EQUAL(unique.partitions()[0].data().get()[0], fill_value);
        }

        // All different values
        {
            Array array{lue::create_partitioned_array<Element>(array_shape, partition_shape)};

            lue::unique_id(array).get();

            auto unique = lue::unique(array).get();

            BOOST_REQUIRE_EQUAL(unique.nr_elements(), lue::nr_elements(array));
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

// TEST_CASE(1, bool) Test uses unique_id, which doesn't support bool
// TEST_CASE(2, bool)
TEST_CASE(1, int32_t)
TEST_CASE(2, int32_t)
// TEST_CASE(1, int64_t)
// TEST_CASE(2, int64_t)
// TEST_CASE(1, float)
// TEST_CASE(2, float)
// TEST_CASE(1, double) // Test uses unique_id, which doesn't support float
// TEST_CASE(2, double)

#undef TEST_CASE
