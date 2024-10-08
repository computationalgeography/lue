#define BOOST_TEST_MODULE lue framework algorithm sum
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/sum.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


namespace {

    template<typename Element, typename ResultElement, std::size_t rank>
    void test_array()
    {
        using Array = lue::PartitionedArray<Element, rank>;
        using Scalar = lue::Scalar<ResultElement>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Element const fill_value{5};

        Array array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};

        auto sum = lue::value_policies::sum<ResultElement>(array);

        using TypeWeGot = decltype(sum);
        using TypeWeWant = Scalar;
        static_assert(std::is_same_v<TypeWeGot, TypeWeWant>);

        BOOST_CHECK_EQUAL(
            sum.future().get(), static_cast<Element>(lue::nr_elements(array_shape) * fill_value));
    }


    template<typename Element, typename ResultElement>
    void test_array_1d()
    {
        test_array<Element, ResultElement, 1>();
    }


    template<typename Element, typename ResultElement>
    void test_array_2d()
    {
        test_array<Element, ResultElement, 2>();
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    test_array_2d<lue::SignedIntegralElement<0>, lue::SignedIntegralElement<0>>();
}
