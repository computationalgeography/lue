#define BOOST_TEST_MODULE lue framework algorithm minimum
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/minimum.hpp"
#include "lue/framework/algorithm/range.hpp"
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

        lue::range(array, Element{5}).get();

        BOOST_CHECK_EQUAL(minimum(array).future().get(), Element{5});
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::Rank const rank{2};

    test_array<lue::LargestSignedIntegralElement, rank>();
    test_array<lue::FloatingPointElement<0>, rank>();
}
