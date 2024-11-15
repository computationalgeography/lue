#define BOOST_TEST_MODULE lue framework algorithm logical_exclusive_or
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/logical_exclusive_or.hpp"
#include "lue/framework/algorithm/default_policies/none.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


namespace {

    template<typename Element, std::size_t rank>
    void test_array()
    {
        if constexpr (lue::BuildOptions::default_policies_enabled)
        {
            using namespace lue::default_policies;

            using BooleanElement = lue::BooleanElement;
            using Array = lue::PartitionedArray<Element, rank>;

            auto const array_shape{lue::Test<Array>::shape()};
            auto const partition_shape{lue::Test<Array>::partition_shape()};

            Element const fill_value1{1};  // true
            Element const fill_value2{0};  // false

            Array array1{lue::create_partitioned_array(array_shape, partition_shape, fill_value1)};
            Array array2{lue::create_partitioned_array(array_shape, partition_shape, fill_value2)};

            // array || array
            {
                BOOST_CHECK(all(logical_exclusive_or<BooleanElement>(array1, array2))
                                .future()
                                .get());  // true || false
                BOOST_CHECK(none(logical_exclusive_or<BooleanElement>(array1, array1))
                                .future()
                                .get());  // true || true
                BOOST_CHECK(none(logical_exclusive_or<BooleanElement>(array2, array2))
                                .future()
                                .get());  // false || false
            }

            // array || scalar
            {
                BOOST_CHECK(none(logical_exclusive_or<BooleanElement>(array1, fill_value1)).future().get());
                BOOST_CHECK(all(logical_exclusive_or<BooleanElement>(array1, fill_value2)).future().get());
            }

            // scalar || array
            {
                BOOST_CHECK(none(logical_exclusive_or<BooleanElement>(fill_value1, array1)).future().get());
                BOOST_CHECK(all(logical_exclusive_or<BooleanElement>(fill_value2, array1)).future().get());
            }
        }
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::Rank const rank{2};

    test_array<lue::BooleanElement, rank>();
}
