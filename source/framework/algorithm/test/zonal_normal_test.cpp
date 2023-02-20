#define BOOST_TEST_MODULE lue framework algorithm zonal_normal
#include "lue/framework/algorithm/array_partition_id.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/all.hpp"
#include "lue/framework/algorithm/value_policies/comparison.hpp"
#include "lue/framework/algorithm/value_policies/zonal_normal.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(use_case_01)
{
    using Element = double;
    using Class = std::uint64_t;
    std::size_t const rank = 2;

    using ElementArray = lue::PartitionedArray<Element, rank>;
    using ClassArray = lue::PartitionedArray<Class, rank>;
    using Shape = lue::ShapeT<ElementArray>;

    Shape const array_shape{{9, 9}};
    Shape const partition_shape{{3, 3}};

    ClassArray class_array{
        lue::array_partition_id(lue::create_partitioned_array<Class>(array_shape, partition_shape))};

    auto zonal_normal = lue::value_policies::zonal_normal<Element>(class_array);

    using namespace lue::value_policies;

    // Very unlikely that values outside (-10, 10) are drawn
    BOOST_CHECK(all(zonal_normal > Element{-10}).get());
    BOOST_CHECK(all(zonal_normal < Element{+10}).get());
}
