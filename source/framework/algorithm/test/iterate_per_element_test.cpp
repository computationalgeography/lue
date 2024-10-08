#define BOOST_TEST_MODULE lue framework algorithm iterate_per_element
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/equal_to.hpp"
#include "lue/framework/algorithm/default_policies/uniform.hpp"
#include "lue/framework/algorithm/iterate_per_element.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


namespace {

    template<typename Element, std::size_t rank>
    void test_array()
    {
        using namespace lue::default_policies;

        using Array = lue::PartitionedArray<Element, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Array array{lue::create_partitioned_array<Element>(array_shape, partition_shape)};

        hpx::shared_future<Element> min_nr_iterations = hpx::make_ready_future<Element>(100);
        hpx::shared_future<Element> max_nr_iterations = hpx::make_ready_future<Element>(500);

        // Request the filling of the array and wait for it to finish
        array = lue::default_policies::uniform(array, min_nr_iterations, max_nr_iterations);

        auto copy = lue::iterate_per_element(array);

        BOOST_CHECK(all(copy == array).future().get());
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::Rank const rank{2};

    if constexpr (sizeof(lue::LargestIntegralElement) > 1)
    {
        test_array<lue::LargestIntegralElement, rank>();
    }
}
