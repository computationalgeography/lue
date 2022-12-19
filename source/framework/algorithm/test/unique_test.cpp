#define BOOST_TEST_MODULE lue framework algorithm unique
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/unique.hpp"
#include "lue/framework/algorithm/value_policies/unique_id.hpp"
#include "lue/framework/algorithm/value_policies/valid.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace detail {

    template<
        typename IDElement,
        std::size_t rank>
    void test_array()
    {
        using Array = lue::PartitionedArray<IDElement, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};
        IDElement const fill_value{5};

        // All the same values
        {
            Array array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};

            auto const unique = lue::value_policies::unique(array).get();

            BOOST_REQUIRE_EQUAL(unique.size(), 1);
            BOOST_CHECK_EQUAL(*unique.cbegin(), fill_value);
        }

        // All different values
        {
            using BooleanElement = std::uint8_t;

            Array array{lue::create_partitioned_array<IDElement>(array_shape, partition_shape, fill_value)};

            auto const unique_id =
                lue::value_policies::unique_id<IDElement>(
                    lue::value_policies::valid<BooleanElement>(
                        array));
            auto const unique = lue::value_policies::unique(unique_id).get();

            BOOST_REQUIRE_EQUAL(unique.size(), lue::nr_elements(array));
        }
    }

}  // namespace detail


#define TEST_CASE(                                 \
    rank,                                          \
    IDElement)                                     \
                                                   \
BOOST_AUTO_TEST_CASE(array_##rank##d_##IDElement)  \
{                                                  \
    detail::test_array<IDElement, rank>();         \
}

// TEST_CASE(1, int32_t)
TEST_CASE(2, int32_t)
TEST_CASE(2, uint64_t)
// TEST_CASE(1, int64_t)
// TEST_CASE(2, int64_t)

#undef TEST_CASE
