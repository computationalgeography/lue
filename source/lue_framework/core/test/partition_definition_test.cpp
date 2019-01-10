#define BOOST_TEST_MODULE lue framework core partition_definition
#include <boost/test/unit_test.hpp>
#include "lue/framework/core/partition_definition.hpp"


BOOST_AUTO_TEST_CASE(construct)
{
    using Index = std::uint32_t;
    std::size_t const rank = 2;
    using Partition = lue::PartitionDefinition<Index, rank>;
    using Start = Partition::Start;
    using Shape = Partition::Shape;

    {
        Partition partition{};

        BOOST_CHECK_EQUAL(partition.start()[0], Index{0});
        BOOST_CHECK_EQUAL(partition.start()[1], Index{0});
        BOOST_CHECK_EQUAL(partition.shape()[0], Index{0});
        BOOST_CHECK_EQUAL(partition.shape()[1], Index{0});
    }

    {
        Partition partition{Shape{30, 40}};

        BOOST_CHECK_EQUAL(partition.start()[0], Index{0});
        BOOST_CHECK_EQUAL(partition.start()[1], Index{0});
        BOOST_CHECK_EQUAL(partition.shape()[0], Index{30});
        BOOST_CHECK_EQUAL(partition.shape()[1], Index{40});
    }

    {
        Partition partition{Start{4, 5}, Shape{30, 40}};

        BOOST_CHECK_EQUAL(partition.start()[0], Index{4});
        BOOST_CHECK_EQUAL(partition.start()[1], Index{5});
        BOOST_CHECK_EQUAL(partition.shape()[0], Index{30});
        BOOST_CHECK_EQUAL(partition.shape()[1], Index{40});
    }
}
