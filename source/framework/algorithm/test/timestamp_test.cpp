#define BOOST_TEST_MODULE lue framework algorithm timestamp
#include "lue/framework/algorithm/default_policies/uniform.hpp"
#include "lue/framework/algorithm/timestamp.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


namespace {

    using Element = lue::LargestIntegralElement;

    constexpr lue::Rank rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    using Count = lue::Count;
    using OutputArray = lue::PartitionedArray<Count, rank>;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(overloads)
{
    if constexpr (lue::BuildOptions::default_policies_enabled)
    {
        Shape const array_shape{{60, 40}};
        Shape const partition_shape{{10, 10}};

        auto array{lue::default_policies::uniform<Element>(array_shape, partition_shape, 0, 100)};

        if constexpr (lue::arithmetic_element_supported<lue::ClockTick>)
        {
            auto result = lue::timestamp(array);
        }
    }
}
