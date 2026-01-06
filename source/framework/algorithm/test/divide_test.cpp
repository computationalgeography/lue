#define BOOST_TEST_MODULE lue framework algorithm divide
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/divide.hpp"
#include "lue/framework/algorithm/default_policies/equal_to.hpp"
#include "lue/framework/algorithm/value_policies/divide.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


namespace {

    template<typename Element, std::size_t rank>
    void test_array()
    {
        if constexpr (lue::BuildOptions::default_policies_enabled)
        {
            using namespace lue::default_policies;

            using Array = lue::PartitionedArray<Element, rank>;

            auto const array_shape{lue::Test<Array>::shape()};
            auto const partition_shape{lue::Test<Array>::partition_shape()};

            Element const fill_value1{15};
            Element const fill_value2{6};

            Array array1{lue::create_partitioned_array(array_shape, partition_shape, fill_value1)};
            Array array2{lue::create_partitioned_array(array_shape, partition_shape, fill_value2)};

            // Divide two arrays
            {
                auto divide = array1 / array2;

                BOOST_CHECK(all(divide == fill_value1 / fill_value2).future().get());
            }

            // Divide scalar by array
            // array / scalar
            {
                auto divide = array1 / fill_value1;

                BOOST_CHECK(all(divide == fill_value1 / fill_value1).future().get());
            }

            // Divide scalar by array
            // scalar / array
            {
                auto divide = fill_value1 / array1;

                BOOST_CHECK(all(divide == fill_value1 / fill_value1).future().get());
            }
        }
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::Rank const rank{2};

    test_array<lue::FloatingPointElement<0>, rank>();
}


BOOST_AUTO_TEST_CASE(overloads)
{
    if constexpr (lue::BuildOptions::default_value_policies_enabled)
    {
        using namespace lue::value_policies;

        // Integral
        {
            using Element = lue::SignedIntegralElement<0>;
            using Scalar = lue::Scalar<Element>;

            Scalar const result_we_got = Scalar{Element{5} / Scalar{Element{2}}};
            Element const result_we_want = Element{5} / Element{2};

            BOOST_CHECK_EQUAL(result_we_got.future().get(), result_we_want);
        }

        // Floating point
        {
            using Element = lue::FloatingPointElement<0>;
            using Scalar = lue::Scalar<Element>;

            Scalar const result_we_got = Scalar{Element{5} / Scalar{Element{2}}};
            Element const result_we_want = Element{5} / Element{2};

            BOOST_CHECK_EQUAL(result_we_got.future().get(), result_we_want);
        }
    }
}
