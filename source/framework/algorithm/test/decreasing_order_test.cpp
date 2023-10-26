#define BOOST_TEST_MODULE lue framework algorithm decreasing_order
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/decreasing_order.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/stream.hpp"


namespace {

    lue::Rank const rank{2};

    template<typename Element>
    using Array = lue::PartitionedArray<Element, rank>;

    using Route = lue::SerialRoute<rank>;

    using Shape = lue::Shape<lue::Count, rank>;  // = typename Array<std::int32_t>::Shape;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(empty_array)
{
    // Create empty array. Verify this works and that the result is also an empty route.
    using namespace lue::value_policies;

    using InputElement = float;

    Shape const array_shape{};
    lue::Count const nr_elements{0};

    Array<InputElement> input_array = lue::create_partitioned_array<InputElement>(array_shape);
    Route route = decreasing_order(input_array);

    BOOST_CHECK_EQUAL(route.shape(), array_shape);
}
