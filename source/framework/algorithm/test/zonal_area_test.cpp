#define BOOST_TEST_MODULE lue framework algorithm zonal_area
#include "lue/framework/algorithm/array_partition_id.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/zonal_area.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(use_case_01)
{
    using Count = lue::CountElement;
    using Class = lue::UnsignedIntegralElement<0>;
    std::size_t const rank = 2;

    using CountArray = lue::PartitionedArray<Count, rank>;
    using ClassArray = lue::PartitionedArray<Class, rank>;
    using Shape = lue::ShapeT<CountArray>;

    Shape const array_shape{{9, 9}};
    Shape const partition_shape{{3, 3}};

    ClassArray class_array{
        lue::array_partition_id<Class>(lue::create_partitioned_array<Class>(array_shape, partition_shape))};

    auto zonal_area = lue::value_policies::zonal_area<Count>(class_array);

    CountArray array_we_want = lue::test::create_partitioned_array<CountArray>(
        array_shape,
        partition_shape,
        {
            {9, 9, 9, 9, 9, 9, 9, 9, 9},
            {9, 9, 9, 9, 9, 9, 9, 9, 9},
            {9, 9, 9, 9, 9, 9, 9, 9, 9},
            {9, 9, 9, 9, 9, 9, 9, 9, 9},
            {9, 9, 9, 9, 9, 9, 9, 9, 9},
            {9, 9, 9, 9, 9, 9, 9, 9, 9},
            {9, 9, 9, 9, 9, 9, 9, 9, 9},
            {9, 9, 9, 9, 9, 9, 9, 9, 9},
            {9, 9, 9, 9, 9, 9, 9, 9, 9},
        });

    lue::test::check_arrays_are_equal(zonal_area, array_we_want);
}
