#define BOOST_TEST_MODULE lue framework algorithm sum
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/sum.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/test/stream.hpp"


LUE_REGISTER_ARRAY_PARTITION(int32_t, 1)


BOOST_AUTO_TEST_CASE(array_1d)
{
    using Value = std::int32_t;
    std::size_t const rank = 1;
    using Data = lue::ArrayPartitionData<Value, rank>;
    using Array = lue::PartitionedArray<std::int32_t, Data>;
    using Shape = typename Data::Shape;
    using Index = typename Data::Index;

    Index const nr_elements = 100;
    Shape const shape{{nr_elements}};

    lue::PartitionedArray<Value, Data> array{shape};
    hpx::shared_future<Value> fill_value = hpx::make_ready_future<Value>(5);

    // Request the filling of the array and wait for it to finish
    lue::fill(array, fill_value).wait();

    // Request the sumation of the array
    auto sum = lue::sum(array);

    BOOST_CHECK_EQUAL(sum.get(), nr_elements * fill_value.get());
}
