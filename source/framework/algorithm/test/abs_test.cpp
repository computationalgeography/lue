#define BOOST_TEST_MODULE lue framework algorithm abs
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/abs.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/equal_to.hpp"
#include "lue/framework/algorithm/value_policies/abs.hpp"
#include "lue/framework/algorithm/value_policies/all.hpp"
#include "lue/framework/algorithm/value_policies/none.hpp"
#include "lue/framework/algorithm/value_policies/valid.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"
#include <limits>


BOOST_AUTO_TEST_CASE(use_case_01)
{
    using Element = lue::FloatingPointElement<0>;
    lue::Rank const rank = 2;
    using namespace lue::default_policies;

    using Array = lue::PartitionedArray<Element, rank>;

    auto const array_shape{lue::Test<Array>::shape()};
    auto const partition_shape{lue::Test<Array>::partition_shape()};

    Array array{lue::create_partitioned_array(array_shape, partition_shape, Element{-5})};

    BOOST_CHECK(all(abs(array) == Element{5}).future().get());
}


BOOST_AUTO_TEST_CASE(out_of_range)
{
    // In 2's complement systems, the absolute value of the smallest integer value cannot
    // be represented. The out of range check must detect this.

    using Element = lue::SignedIntegralElement<0>;
    using BooleanElement = lue::BooleanElement;
    lue::Rank const rank = 2;
    using Array = lue::PartitionedArray<Element, rank>;

    auto const array_shape{lue::Test<Array>::shape()};
    auto const partition_shape{lue::Test<Array>::partition_shape()};

    using namespace lue::value_policies;

    {
        Array array{lue::create_partitioned_array(
            array_shape, partition_shape, Element{std::numeric_limits<Element>::lowest() + 1})};
        BOOST_CHECK(all(valid<BooleanElement>(abs(array))).future().get());
    }

    {
        Array array{lue::create_partitioned_array(
            array_shape, partition_shape, Element{std::numeric_limits<Element>::lowest()})};
        BOOST_CHECK(none(valid<BooleanElement>(abs(array))).future().get());
    }
}
