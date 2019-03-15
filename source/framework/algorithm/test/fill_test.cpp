#define BOOST_TEST_MODULE lue framework algorithm fill
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/sum.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/test/stream.hpp"


BOOST_AUTO_TEST_CASE(array_1d)
{
    using Value = std::int32_t;
    std::size_t const rank = 1;
    using Array = lue::PartitionedArray<Value, rank>;
    using Data = typename Array::Partition::Data;
    using Shape = typename Data::Shape;
    using Index = typename Data::Index;

    Index const nr_elements = 100;
    Shape const shape{{nr_elements}};

    Array array{shape};
    hpx::shared_future<Value> fill_value = hpx::make_ready_future<Value>(5);

    // Request the filling of the array and wait for it to finish
    lue::fill(array, fill_value).wait();


    // ----- REMOVE -----
    for(auto const& partition: array.partitions()) {

        auto const data = partition.data().get();

        for(auto const v: data) {
            BOOST_REQUIRE_EQUAL(v, fill_value.get());
        }
    }
    // ----- REMOVE -----



    // // Request the comparison with the fill value
    // auto equal = lue::equal(array, fill_value);

    // // Request the sumation of the cells that equal the fill value
    // auto sum = lue::sum(equal);

    // // Check whether the number of cells containing the fill value equals
    // // the number of cells in the array
    // BOOST_CHECK_EQUAL(sum.get(), nr_elements);
}
