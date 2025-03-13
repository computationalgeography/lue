#define BOOST_TEST_MODULE lue framework algorithm unique
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/unique.hpp"
#include "lue/framework/algorithm/value_policies/unique_id.hpp"
#include "lue/framework/algorithm/value_policies/valid.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


namespace {

    template<typename IDElement, std::size_t rank>
    void test_array()
    {
        BOOST_TEST_INFO_SCOPE(std::format("test_array<{}, {}>", lue::as_string<IDElement>, rank));

        using Array = lue::PartitionedArray<IDElement, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};
        IDElement const fill_value{5};

        // All the same values
        {
            Array array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};

            auto const unique = lue::value_policies::unique(array).get();

            BOOST_REQUIRE_EQUAL(unique.size(), 1);
            BOOST_CHECK_EQUAL(*unique.cbegin(), fill_value);
        }

        // All different values
        {
            using BooleanElement = lue::BooleanElement;

            Array array{lue::create_partitioned_array<IDElement>(array_shape, partition_shape, fill_value)};

            auto const unique_id =
                lue::value_policies::unique_id<IDElement>(lue::value_policies::valid<BooleanElement>(array));
            auto const unique = lue::value_policies::unique(unique_id).get();

            BOOST_REQUIRE_EQUAL(unique.size(), lue::nr_elements(array));
        }
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::Rank const rank{2};

    test_array<lue::SignedIntegralElement<0>, rank>();
    test_array<lue::UnsignedIntegralElement<0>, rank>();
}
