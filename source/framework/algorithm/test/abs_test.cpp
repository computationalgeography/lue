#define BOOST_TEST_MODULE lue framework algorithm abs
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/abs.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/equal_to.hpp"

#include "lue/framework/algorithm/value_policies/abs.hpp"
#include "lue/framework/algorithm/value_policies/all.hpp"
#include "lue/framework/algorithm/value_policies/none.hpp"
#include "lue/framework/algorithm/value_policies/valid.hpp"

#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include <limits>


namespace detail {

    template<typename Element, std::size_t rank>
    void test_array()
    {
        using namespace lue::default_policies;

        using Array = lue::PartitionedArray<Element, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Array array{lue::create_partitioned_array(array_shape, partition_shape, Element{-5})};

        BOOST_CHECK(all(abs(array) == Element{5}).get());
    }

}  // namespace detail


#define TEST_CASE(rank, Element)                                                                             \
                                                                                                             \
    BOOST_AUTO_TEST_CASE(array_##rank##d_##Element)                                                          \
    {                                                                                                        \
        detail::test_array<Element, rank>();                                                                 \
    }

// TEST_CASE(1, double)
TEST_CASE(2, double)

#undef TEST_CASE


BOOST_AUTO_TEST_CASE(out_of_range)
{
    // In 2's complement systems, the absolute value of the smallest integer value cannot
    // be represented. The out of range check must detect this.

    using Element = std::int32_t;
    using BooleanElement = std::uint8_t;
    lue::Rank const rank = 2;
    using Array = lue::PartitionedArray<Element, rank>;

    auto const array_shape{lue::Test<Array>::shape()};
    auto const partition_shape{lue::Test<Array>::partition_shape()};

    using namespace lue::value_policies;

    {
        Array array{lue::create_partitioned_array(
            array_shape, partition_shape, Element{std::numeric_limits<Element>::lowest() + 1})};
        BOOST_CHECK(all(valid<BooleanElement>(abs(array))).get());
    }

    {
        Array array{lue::create_partitioned_array(
            array_shape, partition_shape, Element{std::numeric_limits<Element>::lowest()})};
        BOOST_CHECK(none(valid<BooleanElement>(abs(array))).get());
    }
}
