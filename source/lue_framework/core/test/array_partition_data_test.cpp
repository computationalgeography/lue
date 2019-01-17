#define BOOST_TEST_MODULE lue framework core array_partition_data
// #include <hpx/config.hpp>
// #include <boost/test/unit_test.hpp>
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/test/stream.hpp"


namespace {

using Index = std::uint32_t;
using Value = std::int32_t;
std::size_t const rank = 2;
using Data = lue::ArrayPartitionData<Index, Value, rank>;
using Definition = typename Data::Definition;
using Start = Definition::Start;
using Shape = Definition::Shape;
using Values = typename Data::Values;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(default_construct)
{
    // Default initialization
    {
        Data data;

        Definition definition{};
        BOOST_CHECK_EQUAL(data.definition(), definition);

        Values values{};
        BOOST_CHECK_EQUAL(data.values(), values);
    }

    // Value intialization
    {
        Data data{};

        Definition definition{};
        BOOST_CHECK_EQUAL(data.definition(), definition);

        Values values{};
        BOOST_CHECK_EQUAL(data.values(), values);
    }
}


BOOST_AUTO_TEST_CASE(construct_initialized_definition)
{
    Definition definition{Start{{4, 5}}, Shape{{30, 40}}};
    Data data{definition};

    BOOST_CHECK_EQUAL(data.definition(), definition);

    Values values{definition.shape()};
    std::fill(values.data(), values.data() + values.num_elements(), 0);
    BOOST_CHECK_EQUAL(data.values(), values);
}


BOOST_AUTO_TEST_CASE(construct_initialized_definition_and_value)
{
    Definition definition{Start{{4, 5}}, Shape{{30, 40}}};
    Data data{definition, 5};

    BOOST_CHECK_EQUAL(data.definition(), definition);

    Values values{definition.shape()};
    std::fill(values.data(), values.data() + values.num_elements(), 5);
    BOOST_CHECK_EQUAL(data.values(), values);
}


BOOST_AUTO_TEST_CASE(assign)
{
    Data data{};

    Definition definition{Start{{4, 5}}, Shape{{30, 40}}};

    data = Data{definition, 5};

    BOOST_CHECK_EQUAL(data.definition(), definition);

    Values values{definition.shape()};
    std::fill(values.data(), values.data() + values.num_elements(), 5);
    BOOST_CHECK_EQUAL(data.values(), values);
}
