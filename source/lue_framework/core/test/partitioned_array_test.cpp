#define BOOST_TEST_MODULE lue framework core partitioned_array
#include "lue/framework/core/partitioned_array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/test/stream.hpp"


namespace {

using Index = std::uint64_t;
using Value = std::int32_t;
static std::size_t const rank = 2;
using PartitionedArray = lue::PartitionedArray<Index, Value, rank>;

// using PartitionClient = lue::client::Partition<Index, Value, rank>;
// using PartitionServer = typename PartitionClient::Server;
// using Data = typename PartitionClient::Data;
using Definition = typename PartitionedArray::Definition;
using Shape = typename Definition::Shape;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(default_construct)
{
    // Default initialization
    {
        PartitionedArray array;

        BOOST_CHECK_EQUAL(array.nr_elements(), 0);

        Definition definition{};
        BOOST_CHECK_EQUAL(array.definition(), definition);

        BOOST_CHECK_EQUAL(array.nr_partitions(), 0);
    }

    // Value initialization
    {
        PartitionedArray array{};

        BOOST_CHECK_EQUAL(array.nr_elements(), 0);

        Definition definition{};
        BOOST_CHECK_EQUAL(array.definition(), definition);

        BOOST_CHECK_EQUAL(array.nr_partitions(), 0);
    }
}


BOOST_AUTO_TEST_CASE(construct_with_definition)
{
    Index nr_rows = 300;
    Index nr_cols = 400;
    Shape shape{{nr_rows, nr_cols}};
    Definition definition{shape};

    PartitionedArray array{definition};

    BOOST_CHECK_EQUAL(array.nr_elements(), nr_rows * nr_cols);
    BOOST_CHECK_EQUAL(array.definition(), definition);

    BOOST_CHECK_EQUAL(array.nr_partitions(), hpx::get_num_localities().get());
}
