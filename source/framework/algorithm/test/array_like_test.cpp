#define BOOST_TEST_MODULE lue framework algorithm array_like
#include "lue/framework/algorithm/array_like.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/equal_to.hpp"
#include "lue/framework/algorithm/locality_id.hpp"
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

        Array input_array{lue::create_partitioned_array<Element>(array_shape, partition_shape)};

        Element const fill_value{5};

        auto output_array = lue::array_like(input_array, fill_value);

        BOOST_CHECK(all(output_array == fill_value).future().get());

        if constexpr (lue::element_supported<std::uint32_t>)
        {
            BOOST_CHECK(all(lue::locality_id(output_array) == lue::locality_id(input_array)).future().get());
        }
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::Rank const rank{2};

    test_array<lue::SignedIntegralElement<0>, rank>();
    test_array<lue::FloatingPointElement<0>, rank>();
}
