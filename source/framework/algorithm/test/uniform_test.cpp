#define BOOST_TEST_MODULE lue framework algorithm uniform
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/comparison.hpp"
#include "lue/framework/algorithm/default_policies/logical_and.hpp"
#include "lue/framework/algorithm/default_policies/uniform.hpp"
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

            Array array1{lue::create_partitioned_array<Element>(array_shape, partition_shape)};
            Array array2{lue::create_partitioned_array<Element>(array_shape, partition_shape)};

            Element min_value{7};
            Element max_value{9};

            // Fill two arrays with values from a uniform distribution and check
            // whether
            // - All cells in these arrays are >= min_value
            // - All cells in these arrays are < max_value
            // (- All cells in these arrays are different)

            {
                array1 = uniform(array1, min_value, max_value);

                // min_value <= array1 < max_value
                BOOST_CHECK(all(array1 >= min_value).future().get());

                if constexpr (std::is_floating_point_v<Element>)
                {
                    BOOST_CHECK(all(array1 < max_value).future().get());
                }
                else if constexpr (std::is_integral_v<Element>)
                {
                    BOOST_CHECK(all(array1 <= max_value).future().get());
                }
            }

            {
                array2 = uniform(array2, min_value, max_value);

                // min_value <= array2 < max_value
                BOOST_CHECK(all(array2 >= min_value).future().get());

                if constexpr (std::is_floating_point_v<Element>)
                {
                    BOOST_CHECK(all(array2 < max_value).future().get());
                }
                else if constexpr (std::is_integral_v<Element>)
                {
                    BOOST_CHECK(all(array2 <= max_value).future().get());
                }
            }

            {
                auto result = uniform(array1, min_value, max_value);

                // min_value <= result < max_value
                BOOST_CHECK(all(result >= min_value).future().get());

                if constexpr (std::is_floating_point_v<Element>)
                {
                    BOOST_CHECK(all(result < max_value).future().get());
                }
                else if constexpr (std::is_integral_v<Element>)
                {
                    BOOST_CHECK(all(result <= max_value).future().get());
                }
            }
        }
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::Rank const rank{2};

    test_array<lue::LargestIntegralElement, rank>();
    test_array<lue::FloatingPointElement<0>, rank>();
}


namespace {

    using Element = lue::LargestIntegralElement;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;
    // using Offset = lue::OffsetT<Array>;

    // Shape const array_shape{{60, 40}};
    // Shape const partition_shape{{10, 10}};
}  // namespace


BOOST_AUTO_TEST_CASE(use_case_1)
{
    // Create a partitioned array filled with random values between two
    // bounds, and verify that all cells contain correct values
    if constexpr (lue::BuildOptions::default_policies_enabled)
    {
        using namespace lue::default_policies;

        lue::Count const nr_rows = 300;
        lue::Count const nr_cols = 400;
        Shape const array_shape{{nr_rows, nr_cols}};

        lue::Count const nr_rows_partition = 43;
        lue::Count const nr_cols_partition = 43;
        Shape const partition_shape{{nr_rows_partition, nr_cols_partition}};

        Element const min_value{33};
        Element const max_value{55};

        Array array = uniform(array_shape, partition_shape, min_value, max_value);

        BOOST_CHECK(all(array >= min_value && array <= max_value).future().get());
    }
}
