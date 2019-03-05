#define BOOST_TEST_MODULE lue framework algorithm fill
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
    using Array = lue::PartitionedArray<Value, Data>;
    using Shape = typename Data::Shape;
    using Index = typename Data::Index;

    Index const nr_elements = 100;
    Shape const shape{{nr_elements}};

    lue::PartitionedArray<Value, Data> array{shape};
    hpx::shared_future<Value> fill_value = hpx::make_ready_future<Value>(5);

    hpx::future<void> result = lue::fill(array, fill_value);

    for(auto const& partition: array.partitions()) {
        auto server = *partition.component();

        for(auto const v: server.data()) {
            BOOST_REQUIRE_EQUAL(v, fill_value.get());
        }
    }

    // TODO Use this one sum is ready

    // auto sum = result.then(
    //     // hpx::launch::async,
    //     [&array](auto&& result) {
    //         return lue::sum(array);
    //     }
    // );

    // BOOST_CHECK_EQUAL(sum.get(), nr_elements * fill_value.get());
}
