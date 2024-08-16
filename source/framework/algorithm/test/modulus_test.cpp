#define BOOST_TEST_MODULE lue framework algorithm modulus
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/modulus.hpp"
#include "lue/framework/algorithm/value_policies/modulus.hpp"
#include "lue/framework/algorithm/value_policies/valid.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace detail {

    template<typename Element, std::size_t rank>
    using Array = lue::PartitionedArray<Element, rank>;

    template<typename Element, std::size_t rank>
    void test_array_modulus_array()
    {
        using namespace lue::value_policies;

        auto const array_shape{lue::Test<Array<Element, rank>>::shape()};
        auto const partition_shape{lue::Test<Array<Element, rank>>::partition_shape()};

        Element const fill_value1{15};
        Element const fill_value2{6};
        Element const fill_value_we_want{15 % 6};

        Array<Element, rank> array1{lue::create_partitioned_array(array_shape, partition_shape, fill_value1)};
        Array<Element, rank> array2{lue::create_partitioned_array(array_shape, partition_shape, fill_value2)};
        Array<Element, rank> array_we_want{
            lue::create_partitioned_array(array_shape, partition_shape, fill_value_we_want)};

        auto modulus = array1 % array2;
        lue::test::check_arrays_are_equal(modulus, array_we_want);
    }

    template<typename Element, std::size_t rank>
    void test_array_modulus_scalar()
    {
        using namespace lue::value_policies;

        auto const array_shape{lue::Test<Array<Element, rank>>::shape()};
        auto const partition_shape{lue::Test<Array<Element, rank>>::partition_shape()};

        Element const fill_value{15};
        Element const scalar_value{-4};
        Element const fill_value_we_want{15 % -4};

        Array<Element, rank> array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};
        Array<Element, rank> array_we_want{
            lue::create_partitioned_array(array_shape, partition_shape, fill_value_we_want)};

        auto modulus = array % scalar_value;
        lue::test::check_arrays_are_equal(modulus, array_we_want);
    }

    template<typename Element, std::size_t rank>
    void test_scalar_modulus_array()
    {
        using namespace lue::value_policies;

        auto const array_shape{lue::Test<Array<Element, rank>>::shape()};
        auto const partition_shape{lue::Test<Array<Element, rank>>::partition_shape()};

        Element const fill_value{2};
        Element const scalar_value{15};
        Element const fill_value_we_want{15 % 2};

        Array<Element, rank> array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};
        Array<Element, rank> array_we_want{
            lue::create_partitioned_array(array_shape, partition_shape, fill_value_we_want)};

        auto modulus = scalar_value % array;
        lue::test::check_arrays_are_equal(modulus, array_we_want);
    }

    template<typename Element, std::size_t rank>
    void test_modulus_nonuniform_array()
    {
        using namespace lue::value_policies;

        using Shape = lue::ShapeT<Array<Element, rank>>;

        Shape const array_shape_specific{{4, 4}};
        Shape const partition_shape_specific{{2, 2}};

        Array<Element, rank> array1 = lue::test::create_partitioned_array<Array<Element, rank>>(
            array_shape_specific,
            partition_shape_specific,

            {
                // clang-format off
                {908, 12, 200, 5},
                {-5, -10, 809, -1},
                {0, 0, 0, 0},
                {5, 5, 5, 5}
                // clang-format on
            });

        Array<Element, rank> array2 = lue::test::create_partitioned_array<Array<Element, rank>>(
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

        Array<Element, rank> array_we_want = lue::test::create_partitioned_array<Array<Element, rank>>(
            array_shape_specific,
            partition_shape_specific,
            {
                // clang-format off
                {908 % 2, 12 % 3, 200 % 6, 5 % 5},
                {-5 % -5, -10 % 10, 809 % -10, -1 % 8},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
                // clang-format on
            });

        auto modulus = array1 % array2;

        lue::test::check_arrays_are_equal(modulus, array_we_want);
    }

    template<typename Element, std::size_t rank>
    void test_modulus_no_data_values()
    {
        using namespace lue::value_policies;

        using Shape = lue::ShapeT<Array<Element, rank>>;

        Shape const array_shape_specific{{4, 4}};
        Shape const partition_shape_specific{{2, 2}};

        Element const nd{lue::policy::no_data_value<Element>};

        {
            // Test case 1: First input array contains no-data values, second input array has values

            Array<Element, rank> array1 = lue::test::create_partitioned_array<Array<Element, rank>>(
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

            Array<Element, rank> array2 = lue::test::create_partitioned_array<Array<Element, rank>>(
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

            Array<Element, rank> array_we_want = lue::test::create_partitioned_array<Array<Element, rank>>(
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

            Array<Element, rank> array1 = lue::test::create_partitioned_array<Array<Element, rank>>(
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

            Array<Element, rank> array2 = lue::test::create_partitioned_array<Array<Element, rank>>(
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

            Array<Element, rank> array_we_want = lue::test::create_partitioned_array<Array<Element, rank>>(
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

}  // namespace detail

BOOST_AUTO_TEST_CASE(array_2d_int32_t_modulus_array)
{
    detail::test_array_modulus_array<int32_t, 2>();
}

BOOST_AUTO_TEST_CASE(array_2d_int32_t_modulus_scalar)
{
    detail::test_array_modulus_scalar<int32_t, 2>();
}

BOOST_AUTO_TEST_CASE(scalar_2d_int32_t_modulus_array)
{
    detail::test_scalar_modulus_array<int32_t, 2>();
}

BOOST_AUTO_TEST_CASE(modulus_2d_int32_t_random_values)
{
    detail::test_modulus_nonuniform_array<int32_t, 2>();
}

BOOST_AUTO_TEST_CASE(modulus_2d_int32_t_no_data_values)
{
    detail::test_modulus_no_data_values<int32_t, 2>();
}
