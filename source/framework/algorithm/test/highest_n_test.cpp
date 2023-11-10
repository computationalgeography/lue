#define BOOST_TEST_MODULE lue framework algorithm highest_n
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/highest_n.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/stream.hpp"


namespace {

    lue::Rank const rank{2};

    template<typename Element>
    using Array = lue::PartitionedArray<Element, rank>;

    using Shape = lue::Shape<lue::Count, rank>;  // = typename Array<std::int32_t>::Shape;

}  // Anonymous namespace


// BOOST_AUTO_TEST_CASE(empty_array)
// {
//     // Create empty array. Verify this works and that the result is also an empty array.
//     using namespace lue::value_policies;
//
//     using InputElement = float;
//     using OutputElement = std::uint8_t;
//
//     Shape const array_shape{};
//     lue::Count const nr_elements{0};
//
//     Array<InputElement> input_array = lue::create_partitioned_array<InputElement>(array_shape);
//     Array<OutputElement> output_array = highest_n<OutputElement>(input_array, nr_elements);
//
//     BOOST_CHECK_EQUAL(output_array.shape(), array_shape);
// }


BOOST_AUTO_TEST_CASE(use_case_01)
{
    BOOST_CHECK(true);
}
