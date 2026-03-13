#define BOOST_TEST_MODULE lue framework algorithm open_simplex_noise
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/all.hpp"
#include "lue/framework/algorithm/value_policies/cast.hpp"
#include "lue/framework/algorithm/value_policies/cell_index.hpp"
#include "lue/framework/algorithm/value_policies/comparison.hpp"
#include "lue/framework/algorithm/value_policies/open_simplex_noise.hpp"
#include "lue/framework/algorithm/value_policies/valid.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


namespace {

    template<typename Element, typename SomeElement, std::size_t rank>
    void test_array()
    {
        if constexpr (lue::BuildOptions::default_value_policies_enabled)
        {
            using namespace lue::value_policies;

            using Array = lue::PartitionedArray<Element, rank>;
            using IndexElement = lue::IndexElement;
            using BooleanElement = lue::BooleanElement;
            using CoordinateElement = lue::FloatingPointElement<0>;

            auto const array_shape{lue::Test<Array>::shape()};
            auto const partition_shape{lue::Test<Array>::partition_shape()};

            auto const condition_array = lue::create_partitioned_array<BooleanElement>(
                array_shape, partition_shape, BooleanElement{1});
            auto const x_index_array = cell_index<IndexElement>(condition_array, 1);
            auto const y_index_array = cell_index<IndexElement>(condition_array, 0);
            auto const x_coordinate_array = cast<CoordinateElement>(x_index_array);
            auto const y_coordinate_array = cast<CoordinateElement>(y_index_array);
            int const seed = 5;

            {
                Array const array = open_simplex_noise<Element>(x_coordinate_array, y_coordinate_array, seed);

                BOOST_CHECK(all(valid<lue::BooleanElement>(array)).future().get());
                BOOST_CHECK(all(array >= Element{-1}).future().get());
                BOOST_CHECK(all(array <= Element{1}).future().get());
            }
        }
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::Rank const rank{2};

    test_array<lue::FloatingPointElement<0>, lue::SmallestIntegralElement, rank>();
}
