#define BOOST_TEST_MODULE lue framework algorithm zonal_uniform
#include "lue/framework/algorithm/array_partition_id.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/all.hpp"
#include "lue/framework/algorithm/value_policies/comparison.hpp"
#include "lue/framework/algorithm/value_policies/zonal_uniform.hpp"
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

    auto zonal_uniform = lue::value_policies::zonal_uniform<Element>(class_array);

    using namespace lue::value_policies;

    BOOST_CHECK(all(zonal_uniform >= Element{0}).future().get());
    BOOST_CHECK(all(zonal_uniform < Element{1}).future().get());
}
