#define BOOST_TEST_MODULE lue framework algorithm greater_than_equal_to
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/greater_than_equal_to.hpp"
#include "lue/framework/algorithm/default_policies/none.hpp"
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

        Element const fill_value1{5};
        Element const fill_value2{6};

        Array array1{lue::create_partitioned_array(array_shape, partition_shape, fill_value1)};
        Array array2{lue::create_partitioned_array(array_shape, partition_shape, fill_value2)};

        // Compare two arrays with different values
        {
            BOOST_CHECK(all(array2 >= array1).future().get());
            BOOST_CHECK(none(array1 >= array2).future().get());
        }

        // Compare two arrays with the same values
        {
            BOOST_CHECK(all(array2 >= array2).future().get());
        }

        // Compare array with scalar
        // array >= scalar
        {
            BOOST_CHECK(all(array2 >= fill_value2).future().get());
            BOOST_CHECK(none(array1 >= fill_value2).future().get());
        }

        // Compare array with scalar
        // scalar >= array
        {
            BOOST_CHECK(all(fill_value2 >= array2).future().get());
            BOOST_CHECK(none(fill_value1 >= array2).future().get());
        }
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::Rank const rank{2};

    test_array<lue::SignedIntegralElement<0>, rank>();
    test_array<lue::FloatingPointElement<0>, rank>();
}
