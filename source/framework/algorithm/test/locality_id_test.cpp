#define BOOST_TEST_MODULE lue framework algorithm locality_id
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/locality_id.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


namespace {

    template<typename Element, std::size_t rank>
    void test_array()
    {
        using Array = lue::PartitionedArray<Element, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Array array{lue::create_partitioned_array<Element>(array_shape, partition_shape)};

        // Request the locality IDs of each partition
        auto locality_id = lue::locality_id(array);

        BOOST_CHECK_EQUAL(locality_id.shape(), array.partitions().shape());
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::Rank const rank{2};

    if constexpr (lue::element_supported<std::uint32_t>)
    {
        test_array<lue::SignedIntegralElement<0>, rank>();
    }
}
