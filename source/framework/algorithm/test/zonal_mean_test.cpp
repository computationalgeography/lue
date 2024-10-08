#define BOOST_TEST_MODULE lue framework algorithm zonal_mean
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/zonal_mean.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(use_case_01)
{
    using Value = lue::FloatingPointElement<0>;
    using Class = lue::UnsignedIntegralElement<0>;
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
            -9, 0, -6, -6, -6, 1, 1, -6, -6, vx, 1, 1, -1, 7, 2, 1, 1, 3, 5, 8, 8, vx, vx, 2.5, 1.4,
        }});

    ClassArray class_array = lue::test::create_partitioned_array<ClassArray>(
        array_shape,
        partition_shape,
        {{
            2, 6, 2, 2, cx, 6, 6, 2, 2, 2, 6, 6, 0, 0, 0, 6, 6, 0, 0, 0, 6, 3, 3, 4, 4,
        }});

    auto zonal_mean = lue::value_policies::zonal_mean(value_array, class_array);

    ValueArray array_we_want = lue::test::create_partitioned_array<ValueArray>(
        array_shape,
        partition_shape,
        {{
            -6.6, 1.75, -6.6, -6.6, vx, 1.75, 1.75, -6.6, -6.6, -6.6, 1.75, 1.75, 4,
            4,    4,    1.75, 1.75, 4,  4,    4,    1.75, vx,   vx,   1.95, 1.95,
        }});

    lue::test::check_arrays_are_equal(zonal_mean, array_we_want);
}
