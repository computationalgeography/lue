#define BOOST_TEST_MODULE lue framework algorithm close_to
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/close_to.hpp"
#include "lue/framework/algorithm/default_policies/none.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


namespace {

    template<typename Element, std::size_t rank>
    void test_array()
    {
        using namespace lue::default_policies;

        using BooleanElement = lue::BooleanElement;
        using Array = lue::PartitionedArray<Element, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Element const fill_value1{5};
        Element const fill_value2{6};

        Array array1{lue::create_partitioned_array(array_shape, partition_shape, fill_value1)};
        Array array2{lue::create_partitioned_array(array_shape, partition_shape, fill_value2)};

        // Compare two arrays with different values
        {
            BOOST_CHECK(none(close_to<BooleanElement>(array1, array2)).future().get());
        }

        // Compare two arrays with the same values
        {
            BOOST_CHECK(all(close_to<BooleanElement>(array1, array1)).future().get());
        }

        // Compare array with scalar
        // array == scalar
        {
            BOOST_CHECK(all(close_to<BooleanElement>(array1, fill_value1)).future().get());
        }

        // Compare array with scalar
        // scalar == array
        {
            BOOST_CHECK(all(close_to<BooleanElement>(fill_value1, array1)).future().get());
        }

        // TODO Add tests with close values
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::Rank const rank{2};

    test_array<lue::FloatingPointElement<0>, rank>();
}
