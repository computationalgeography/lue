#define BOOST_TEST_MODULE lue framework core array_partition_definition
#include <boost/test/unit_test.hpp>
#include "lue/framework/core/array_partition_definition.hpp"


namespace {

using Index = std::uint32_t;
std::size_t const rank = 2;
using Definition = lue::ArrayPartitionDefinition<Index, rank>;
using Start = Definition::Start;
using Shape = Definition::Shape;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(default_construct)
{
    // Default initialization
    {
        Definition definition;

        BOOST_CHECK_EQUAL(definition.start()[0], Index{0});
        BOOST_CHECK_EQUAL(definition.start()[1], Index{0});
        BOOST_CHECK_EQUAL(definition.shape()[0], Index{0});
        BOOST_CHECK_EQUAL(definition.shape()[1], Index{0});

        BOOST_CHECK_EQUAL(definition.nr_elements(), 0);
    }

    // Value intialization
    {
        Definition definition{};

        BOOST_CHECK_EQUAL(definition.start()[0], Index{0});
        BOOST_CHECK_EQUAL(definition.start()[1], Index{0});
        BOOST_CHECK_EQUAL(definition.shape()[0], Index{0});
        BOOST_CHECK_EQUAL(definition.shape()[1], Index{0});

        BOOST_CHECK_EQUAL(definition.nr_elements(), 0);
    }

}


BOOST_AUTO_TEST_CASE(construct_initialized_with_shape)
{
    Definition definition{Shape{30, 40}};

    BOOST_CHECK_EQUAL(definition.start()[0], Index{0});
    BOOST_CHECK_EQUAL(definition.start()[1], Index{0});
    BOOST_CHECK_EQUAL(definition.shape()[0], Index{30});
    BOOST_CHECK_EQUAL(definition.shape()[1], Index{40});

    BOOST_CHECK_EQUAL(definition.nr_elements(), 30 * 40);
}


BOOST_AUTO_TEST_CASE(construct_initialized_with_start_shape)
{
    Definition definition{Start{4, 5}, Shape{30, 40}};

    BOOST_CHECK_EQUAL(definition.start()[0], Index{4});
    BOOST_CHECK_EQUAL(definition.start()[1], Index{5});
    BOOST_CHECK_EQUAL(definition.shape()[0], Index{30});
    BOOST_CHECK_EQUAL(definition.shape()[1], Index{40});

    BOOST_CHECK_EQUAL(definition.nr_elements(), 30 * 40);
}
