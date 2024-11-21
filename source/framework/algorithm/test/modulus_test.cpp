#define BOOST_TEST_MODULE lue framework algorithm modulus
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/equal_to.hpp"
#include "lue/framework/algorithm/default_policies/modulus.hpp"
#include "lue/framework/algorithm/value_policies/modulus.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


namespace {

    template<typename Element, std::size_t rank>
    void test_array()
    {
        if constexpr (lue::BuildOptions::default_policies_enabled)
        {
            using namespace lue::default_policies;

            using Scalar = lue::Scalar<Element>;
            using Array = lue::PartitionedArray<Element, rank>;

            auto const array_shape{lue::Test<Array>::shape()};
            auto const partition_shape{lue::Test<Array>::partition_shape()};

            Element const value1{15};
            Element const value2{6};
            Element const value_we_want{15 % 6};

            Scalar scalar1{value1};
            Scalar scalar2{value2};

            Array array1{lue::create_partitioned_array(array_shape, partition_shape, value1)};
            Array array2{lue::create_partitioned_array(array_shape, partition_shape, value2)};

            BOOST_CHECK(all(array1 % array2 == value_we_want).future().get());
            BOOST_CHECK(all(array1 % scalar2 == value_we_want).future().get());
            BOOST_CHECK(all(array1 % value2 == value_we_want).future().get());

            BOOST_CHECK(all(scalar1 % array2 == value_we_want).future().get());
            BOOST_CHECK(all(value1 % array2 == value_we_want).future().get());

            BOOST_CHECK((scalar1 % scalar2 == value_we_want).future().get());
            BOOST_CHECK((scalar1 % value2 == value_we_want).future().get());

            BOOST_CHECK((value1 % scalar2 == value_we_want).future().get());
        }
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::Rank const rank{2};

    test_array<lue::SignedIntegralElement<0>, rank>();
}


BOOST_AUTO_TEST_CASE(modulus_2d_random_values)
{
    using namespace lue::value_policies;

    lue::Rank const rank{2};

    using Element = lue::SignedIntegralElement<0>;
    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape_specific{{4, 4}};
    Shape const partition_shape_specific{{2, 2}};

    auto array1 = lue::test::create_partitioned_array<Array>(
        array_shape_specific,
        partition_shape_specific,

        {
            // clang-format off
            {98, 12, 20, 5},
            {-5, -10, 89, -1},
            {0, 0, 0, 0},
            {5, 5, 5, 5}
            // clang-format on
        });

    auto array2 = lue::test::create_partitioned_array<Array>(
        array_shape_specific,
        partition_shape_specific,
        {
            // clang-format off
            {2, 3, 6, 5},
            {-5, 10, -10, 8},
            {2, 2, 2, 2},
            {5, 5, 5, 5}
            // clang-format on
        });

    auto array_we_want = lue::test::create_partitioned_array<Array>(
        array_shape_specific,
        partition_shape_specific,
        {
            // clang-format off
            {98 % 2, 12 % 3, 20 % 6, 5 % 5},
            {-5 % -5, -10 % 10, 89 % -10, -1 % 8},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
            // clang-format on
        });

    auto modulus = array1 % array2;

    lue::test::check_arrays_are_equal(modulus, array_we_want);
}


BOOST_AUTO_TEST_CASE(modulus_2d_no_data_values)
{
    using namespace lue::value_policies;

    lue::Rank const rank{2};

    using Element = lue::SignedIntegralElement<0>;
    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape_specific{{4, 4}};
    Shape const partition_shape_specific{{2, 2}};

    Element const nd{lue::policy::no_data_value<Element>};

    {
        // Test case 1: First input array contains no-data values, second input array has values

        auto array1 = lue::test::create_partitioned_array<Array>(
            array_shape_specific,
            partition_shape_specific,
            {
                // clang-format off
            {nd, nd, nd, nd},
            {nd, nd, nd, nd},
            {nd, nd, nd, nd},
            {nd, nd, nd, nd}
                // clang-format on
            });

        auto array2 = lue::test::create_partitioned_array<Array>(
            array_shape_specific,
            partition_shape_specific,
            {
                // clang-format off
            {2, 3, 6, 5},
            {-5, 10, -10, 8},
            {0, 0, 0, 0},
            {nd, nd, nd, nd}
                // clang-format on
            });

        auto array_we_want = lue::test::create_partitioned_array<Array>(
            array_shape_specific,
            partition_shape_specific,
            {
                // clang-format off
            {nd, nd, nd, nd},
            {nd, nd, nd, nd},
            {nd, nd, nd, nd},
            {nd, nd, nd, nd}
                // clang-format on
            });

        auto modulus = array1 % array2;

        lue::test::check_arrays_are_equal(modulus, array_we_want);
    }

    {
        // Test case 2: First input array has values, second input array contains no-data values

        auto array1 = lue::test::create_partitioned_array<Array>(
            array_shape_specific,
            partition_shape_specific,
            {
                // clang-format off
            {2, 3, 6, 5},
            {-5, 10, -10, 8},
            {0, 0, 0, 0},
            {7, 8, 9, 10}
                // clang-format on
            });

        auto array2 = lue::test::create_partitioned_array<Array>(
            array_shape_specific,
            partition_shape_specific,
            {
                // clang-format off
            {nd, nd, nd, nd},
            {nd, nd, nd, nd},
            {nd, nd, nd, nd},
            {nd, nd, nd, nd}
                // clang-format on
            });

        auto array_we_want = lue::test::create_partitioned_array<Array>(
            array_shape_specific,
            partition_shape_specific,
            {
                // clang-format off
            {nd, nd, nd, nd},
            {nd, nd, nd, nd},
            {nd, nd, nd, nd},
            {nd, nd, nd, nd}
                // clang-format on
            });

        auto modulus = array1 % array2;
        lue::test::check_arrays_are_equal(modulus, array_we_want);
    }
}
