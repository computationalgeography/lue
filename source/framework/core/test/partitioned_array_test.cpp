#define BOOST_TEST_MODULE lue framework core partitioned_array
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/test/stream.hpp"


namespace {

using Value = std::int32_t;
static std::size_t const rank = 2;
using PartitionedArray = lue::PartitionedArray<Value, rank>;
using Data = typename PartitionedArray::Partition::Data;

// using PartitionClient = lue::client::Partition<Index, Value, rank>;
// using PartitionServer = typename PartitionClient::Server;
// using Data = typename PartitionClient::Data;

// using Definition = typename PartitionedArray::Definition;
using Index = typename Data::Index;
using Shape = typename Data::Shape;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(default_construct)
{
    // Default initialization
    {
        PartitionedArray array;

        BOOST_CHECK_EQUAL(array.nr_elements(), 0u);

        Shape shape{};
        BOOST_CHECK_EQUAL(array.shape(), shape);

        BOOST_CHECK_EQUAL(array.nr_partitions(), 0u);
    }

    // Value initialization
    {
        PartitionedArray array{};

        BOOST_CHECK_EQUAL(array.nr_elements(), 0u);

        Shape shape{};
        BOOST_CHECK_EQUAL(array.shape(), shape);

        BOOST_CHECK_EQUAL(array.nr_partitions(), 0u);
    }
}


BOOST_AUTO_TEST_CASE(construct_with_shape)
{
    Index nr_rows = 300;
    Index nr_cols = 400;
    Shape shape{{nr_rows, nr_cols}};

    PartitionedArray array{shape};

    BOOST_CHECK_EQUAL(array.nr_elements(), nr_rows * nr_cols);
    BOOST_CHECK_EQUAL(array.shape(), shape);
    BOOST_CHECK_EQUAL(array.nr_partitions(), hpx::get_num_localities().get());
}


BOOST_AUTO_TEST_CASE(construct_with_max_partition_shape)
{
    Index nr_rows = 30;
    Index nr_cols = 40;
    Shape shape{{nr_rows, nr_cols}};

    {
        // 10 x 10 partitions, without clamping
        Index nr_rows_partition = 3;
        Index nr_cols_partition = 4;
        Shape max_partition_shape{{nr_rows_partition, nr_cols_partition}};

        PartitionedArray array{shape, max_partition_shape};

        BOOST_CHECK_EQUAL(array.nr_elements(), nr_rows * nr_cols);
        BOOST_CHECK_EQUAL(array.shape(), shape);
        BOOST_CHECK_EQUAL(array.nr_partitions(), 100u);
    }

    {
        // 1 x 1 partition, without clamping
        Index nr_rows_partition = 30;
        Index nr_cols_partition = 40;
        Shape max_partition_shape{{nr_rows_partition, nr_cols_partition}};

        PartitionedArray array{shape, max_partition_shape};

        BOOST_CHECK_EQUAL(array.nr_elements(), nr_rows * nr_cols);
        BOOST_CHECK_EQUAL(array.shape(), shape);
        BOOST_CHECK_EQUAL(array.nr_partitions(), 1u);
    }

    {
        // 2 x 2 partitions, with clamping
        Index nr_rows_partition = 20;
        Index nr_cols_partition = 30;
        Shape max_partition_shape{{nr_rows_partition, nr_cols_partition}};

        PartitionedArray array{shape, max_partition_shape};

        BOOST_CHECK_EQUAL(array.nr_elements(), nr_rows * nr_cols);
        BOOST_CHECK_EQUAL(array.shape(), shape);
        BOOST_CHECK_EQUAL(array.nr_partitions(), 4u);

        auto const& partitions = array.partitions();
        BOOST_CHECK_EQUAL(partitions[0].shape().get(), max_partition_shape);
        BOOST_CHECK_EQUAL(partitions[1].shape().get(), Shape({{20, 10}}));
        BOOST_CHECK_EQUAL(partitions[2].shape().get(), Shape({{10, 30}}));
        BOOST_CHECK_EQUAL(partitions[3].shape().get(), Shape({{10, 10}}));
    }
}
