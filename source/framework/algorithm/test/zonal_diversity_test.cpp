#define BOOST_TEST_MODULE lue framework algorithm zonal_diversity
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/zonal_diversity.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(use_case_01)
{
    using Count = std::uint64_t;
    using Value = std::int32_t;
    using Class = std::uint8_t;
    std::size_t const rank = 2;

    using CountArray = lue::PartitionedArray<Count, rank>;
    using ValueArray = lue::PartitionedArray<Value, rank>;
    using ClassArray = lue::PartitionedArray<Class, rank>;
    using Shape = lue::ShapeT<ValueArray>;

    Shape const array_shape{{5, 5}};
    Shape const partition_shape{{5, 5}};

    // Example from the PCRaster manual

    Count const ox{lue::policy::no_data_value<Count>};  // Count no-data
    Class const cx{lue::policy::no_data_value<Class>};  // Class no-data
    Value const vx{lue::policy::no_data_value<Value>};  // Value no-data
    ValueArray value_array = lue::test::create_partitioned_array<ValueArray>(
        array_shape,
        partition_shape,
        {{
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 7, 6, 0, 4, 18, 2, 6, 2, 8, -1, 5, 2, 3, 3, vx,
        }});

    ClassArray class_array = lue::test::create_partitioned_array<ClassArray>(
        array_shape,
        partition_shape,
        {{
            2, 6, 2, 2, cx, 6, 6, 2, 2, 2, 6, 6, 0, 0, 0, 6, 6, 0, 0, 0, 6, 3, 3, 4, 4,
        }});

    auto zonal_diversity = lue::value_policies::zonal_diversity<Count>(value_array, class_array);

    CountArray array_we_want = lue::test::create_partitioned_array<CountArray>(
        array_shape,
        partition_shape,
        {{
            1, 5, 1, 1, ox, 5, 5, 1, 1, 1, 5, 5, 6, 6, 6, 5, 5, 6, 6, 6, 5, 2, 2, 1, 1,
        }});

    lue::test::check_arrays_are_equal(zonal_diversity, array_we_want);
}
