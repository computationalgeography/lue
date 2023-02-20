#define BOOST_TEST_MODULE lue framework algorithm zonal_majority
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/zonal_majority.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(use_case_01)
{
    using Value = std::int32_t;
    using Class = std::uint8_t;
    std::size_t const rank = 2;

    using ValueArray = lue::PartitionedArray<Value, rank>;
    using ClassArray = lue::PartitionedArray<Class, rank>;
    using Shape = lue::ShapeT<ValueArray>;

    Shape const array_shape{{5, 5}};
    Shape const partition_shape{{5, 5}};

    // Example from the PCRaster manual

    Class const cx{lue::policy::no_data_value<Class>};  // Class no-data
    Value const vx{lue::policy::no_data_value<Value>};  // Value no-data

    ValueArray value_array = lue::test::create_partitioned_array<ValueArray>(
        array_shape,
        partition_shape,
        {{
            vx, 1, 1, 1, 1, -6, -6, 18, 1, 0, -6, -6, -6, 0, 0, -6, -6, -6, 0, 4, 0, 4, 0, 1, -6,
        }});

    ClassArray class_array = lue::test::create_partitioned_array<ClassArray>(
        array_shape,
        partition_shape,
        {{
            2, 6, 2, 2, cx, 6, 6, 2, 2, 2, 6, 6, 0, 0, 0, 6, 6, 0, 0, 0, 6, 3, 3, 4, 4,
        }});

    auto zonal_majority = lue::value_policies::zonal_majority(value_array, class_array);

    ValueArray array_we_want = lue::test::create_partitioned_array<ValueArray>(
        array_shape,
        partition_shape,
        {{
            1, -6, 1, 1, vx, -6, -6, 1, 1, 1, -6, -6, 0, 0, 0, -6, -6, 0, 0, 0, -6, 4, 4, 1, 1,
        }});

    lue::test::check_arrays_are_equal(zonal_majority, array_we_want);
}
