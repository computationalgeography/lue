#define BOOST_TEST_MODULE lue framework algorithm add
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/add.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/equal_to.hpp"
#include "lue/framework/algorithm/value_policies/add.hpp"
#include "lue/framework/algorithm/value_policies/none.hpp"
#include "lue/framework/algorithm/value_policies/valid.hpp"
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

        Array add = array1 + array2;

        BOOST_CHECK(all(add == fill_value1 + fill_value2).get());
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


BOOST_AUTO_TEST_CASE(out_of_range)
{
    using namespace lue::value_policies;

    using Element = std::int32_t;
    lue::Rank const rank{2};
    using Scalar = lue::Scalar<Element>;
    using Array = lue::PartitionedArray<Element, rank>;

    auto const array_shape{lue::Test<Array>::shape()};
    auto const partition_shape{lue::Test<Array>::partition_shape()};

    Element const max{std::numeric_limits<Element>::max()};
    Element const fill_value{max};

    Array array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};

    // Careful. The default no-data value for int32_t is the lowest value. Although max +
    // 1 is undefined for signed integers, this may wrap around to this minimum value. Therefore,
    // we add 2 here, instead of 1. Adding 1 confuses things.
    BOOST_CHECK(none(valid<std::uint8_t>(Scalar{2} + array)).get());
    BOOST_CHECK(none(valid<std::uint8_t>(array + Scalar{2})).get());
    BOOST_CHECK(none(valid<std::uint8_t>(array + array)).get());
    BOOST_CHECK_EQUAL((valid<std::uint8_t>(Scalar{max} + Scalar{2})).value().get(), 0);
}
