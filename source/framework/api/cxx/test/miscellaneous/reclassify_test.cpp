#define BOOST_TEST_MODULE lue framework api cxx reclassify
#include "unit_test.hpp"
#include "lue/framework/api/cxx/miscellaneous/reclassify.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(reclassify)
{
    using InputElement = lue::SignedIntegralElement<0>;
    using InputArray = lue::PartitionedArray<InputElement, 2>;
    using OutputElement = lue::FloatingPointElement<0>;
    using OutputArray = lue::PartitionedArray<OutputElement, 2>;
    using LookupTable = lue::LookupTable<InputElement, OutputElement>;

    lue::api::Field const raster = InputArray{};
    lue::api::LookupTable const lookup_table = LookupTable{};
    lue::api::Field result = lue::api::reclassify(raster, lookup_table);

    BOOST_CHECK(std::holds_alternative<OutputArray>(result.variant()));
}
