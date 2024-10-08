#define BOOST_TEST_MODULE lue framework algorithm unique_id
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/unique.hpp"
#include "lue/framework/algorithm/default_policies/unique_id.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


namespace {

    template<typename IDElement, typename ConditionElement, std::size_t rank>
    void test_array()
    {
        using Array = lue::PartitionedArray<ConditionElement, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Array array{lue::create_partitioned_array<ConditionElement>(array_shape, partition_shape, 1)};

        {
            auto unique_id = lue::default_policies::unique_id<IDElement>(array);
            auto unique = lue::default_policies::unique(unique_id).get();

            BOOST_REQUIRE_EQUAL(unique.size(), lue::nr_elements(array));
        }
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::Rank const rank{2};

    test_array<lue::SignedIntegralElement<0>, lue::BooleanElement, rank>();
}
