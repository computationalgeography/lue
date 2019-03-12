#define BOOST_TEST_MODULE lue framework algorithm sum
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/sum.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/test/stream.hpp"


BOOST_AUTO_TEST_CASE(array_1d_int32_int32)
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


// BOOST_AUTO_TEST_CASE(array_1d_int64_int64)
// {
//     using Element = std::int64_t;
//     using ResultElement = std::int64_t;
//     std::size_t const rank = 1;
//     using Data = lue::ArrayPartitionData<Element, rank>;
//     using Array = lue::PartitionedArray<Element, Data>;
//     using Shape = typename Data::Shape;
//     using Index = typename Data::Index;
// 
//     Index const nr_elements = 100;
//     Shape const shape{{nr_elements}};
// 
//     lue::PartitionedArray<Element, Data> array{shape};
//     hpx::shared_future<Element> fill_value =
//         hpx::make_ready_future<Element>(5);
// 
//     // Request the filling of the array and wait for it to finish
//     lue::fill(array, fill_value).wait();
// 
//     // Request the sumation of the array
//     auto sum = lue::sum<Array, ResultElement>(array);
// 
//     using TypeWeGot = decltype(sum);
//     using TypeWeWant = hpx::future<ResultElement>;
//     static_assert(std::is_same_v<TypeWeGot, TypeWeWant>);
// 
//     // TODO Booom!
//     BOOST_CHECK_EQUAL(sum.get(), nr_elements * fill_value.get());
// }


BOOST_AUTO_TEST_CASE(array_1d_int32_int64)
{
    using Element = std::int32_t;
    using ResultElement = std::int64_t;
    std::size_t const rank = 1;
    using Data = lue::ArrayPartitionData<Element, rank>;
    using Array = lue::PartitionedArray<Element, Data>;
    using Shape = typename Data::Shape;
    using Index = typename Data::Index;

    Index const nr_elements = 100;
    Shape const shape{{nr_elements}};

    Array array{shape};
    hpx::shared_future<Element> fill_value =
        hpx::make_ready_future<Element>(5);

    // Request the filling of the array and wait for it to finish
    lue::fill(array, fill_value).wait();

    // Request the sumation of the array
    auto sum = lue::sum<Array, ResultElement>(array);

    using TypeWeGot = decltype(sum);
    using TypeWeWant = hpx::future<ResultElement>;
    static_assert(std::is_same_v<TypeWeGot, TypeWeWant>);

    BOOST_CHECK_EQUAL(sum.get(), nr_elements * fill_value.get());
}
