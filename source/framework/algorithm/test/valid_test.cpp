#define BOOST_TEST_MODULE lue framework algorithm valid
#include "lue/framework/algorithm/all.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/definition/equal_to.hpp"
#include "lue/framework/algorithm/definition/not_equal_to.hpp"
#include "lue/framework/algorithm/valid.hpp"
#include "lue/framework/algorithm/definition/where.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

    using Element = std::int32_t;
    using BooleanElement = std::uint8_t;
    constexpr lue::Rank rank{2};

    using Array = lue::PartitionedArray<Element, rank>;
    using BooleanArray = lue::PartitionedArray<BooleanElement, rank>;

    auto const array_shape{lue::Test<Array>::shape()};
    auto const partition_shape{lue::Test<Array>::partition_shape()};

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(default_policies)
{
    // Default policies: all data are valid

    {
        Element const fill_value{0};
        Array array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};
        BooleanArray result = lue::valid<BooleanElement>(array);
        BOOST_CHECK(lue::all(result == BooleanElement{1}).get());
    }

    {
        Element const fill_value{-1};
        Array array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};
        BooleanArray result = lue::valid<BooleanElement>(array);
        BOOST_CHECK(lue::all(result == BooleanElement{1}).get());
    }

    {
        Element const fill_value{1};
        Array array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};
        BooleanArray result = lue::valid<BooleanElement>(array);
        BOOST_CHECK(lue::all(result == BooleanElement{1}).get());
    }
}


BOOST_AUTO_TEST_CASE(default_value_policies)
{
    // Default value policies: all non-no-data elements are valid

    using ValidPolicies = lue::policy::valid::DefaultValuePolicies<BooleanElement, Element>;
    using WherePolicies = lue::policy::where::DefaultValuePolicies<Element, BooleanElement, Element>;

    {
        // All valid data
        Element const fill_value{1};
        Array array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};
        array = lue::where<WherePolicies, BooleanElement>(WherePolicies{}, array == fill_value, array);
        BooleanArray result = lue::valid<BooleanElement>(ValidPolicies{}, array);
        BOOST_CHECK(lue::all(result == BooleanElement{1}).get());
    }

    {
        // All invalid data
        Element const fill_value{1};
        Array array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};
        array = lue::where<WherePolicies, BooleanElement>(WherePolicies{}, array != fill_value, array);
        BooleanArray result = lue::valid<BooleanElement>(ValidPolicies{}, array);
        BOOST_CHECK(lue::all(result == BooleanElement{0}).get());
    }
}
