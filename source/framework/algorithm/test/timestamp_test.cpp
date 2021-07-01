#define BOOST_TEST_MODULE lue framework algorithm timestamp
#include "lue/framework/algorithm/uniform.hpp"
#include "lue/framework/algorithm/timestamp.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

    using Element = std::int32_t;
    constexpr lue::Rank rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{60, 40}};
    Shape const partition_shape{{10, 10}};

    using Count = lue::Count;
    using OutputArray = lue::PartitionedArray<Count, rank>;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(overloads)
{
    {
        auto array{lue::uniform<std::int32_t>(array_shape, partition_shape, 0, 100)};

        auto result = lue::timestamp(array);
    }
}
