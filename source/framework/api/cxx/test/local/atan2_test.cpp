#define BOOST_TEST_MODULE lue framework api cxx local atan2
#include "lue/framework/api/cxx/local/atan2.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(atan2_array_array)
{
    using Element = lue::SmallestFloatingPointElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const array = Array{};

    {
        lue::api::Field result = lue::api::atan2(array, array);

        BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
    }
}
