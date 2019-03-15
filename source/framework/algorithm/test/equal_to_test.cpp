#define BOOST_TEST_MODULE lue framework algorithm equal_to
#include "lue/framework/algorithm/equal_to.hpp"
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/sum.hpp"
#include "lue/framework/define.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/test/stream.hpp"


// LUE_PLAIN_ACTION(bool, 1)

// LUE_PLAIN_ACTION_SPECIALIZATION(
//     lue, sum_partition3, SumPartitionAction3, int32_t, int32_t)


BOOST_AUTO_TEST_CASE(array_array_1d)
{
    // using Value = std::int32_t;
    // std::size_t const rank = 1;
    // using Data = lue::ArrayPartitionData<Value, rank>;
    // using Array = lue::PartitionedArray<std::int32_t, Data>;
    // using Shape = typename Data::Shape;
    // using Index = typename Data::Index;

    // Index const nr_elements = 100;
    // Shape const shape{{nr_elements}};

    // lue::PartitionedArray<Value, Data> array1{shape};
    // lue::PartitionedArray<Value, Data> array2{shape};

    // // Compare arrays with different values
    // {
    //     hpx::shared_future<Value> fill_value1 =
    //         hpx::make_ready_future<Value>(5);
    //     hpx::shared_future<Value> fill_value2 =
    //         hpx::make_ready_future<Value>(6);

    //     hpx::future<void> filled1 = lue::fill(array1, fill_value1);
    //     hpx::future<void> filled2 = lue::fill(array2, fill_value2);
    //     hpx::wait_all(filled1, filled2);

    //     auto equal_to = lue::equal_to(array1, array2);
    //     // auto sum = lue::sum(equal_to);

    //     // BOOST_CHECK_EQUAL(sum.get(), 0);
    // }

    // // Compare arrays with the same values
    // {
    //     auto equal_to = lue::equal_to(array1, array1);
    //     // auto sum = lue::sum(equal_to);

    //     // BOOST_CHECK_EQUAL(sum.get(), nr_elements);
    // }
}
