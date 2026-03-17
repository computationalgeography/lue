#define BOOST_TEST_MODULE lue framework partitioned_array partitioned_array
#include "lue/framework/core/component.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

    using Value = std::int32_t;
    static lue::Rank const rank = 2;
    using PartitionedArray = lue::PartitionedArray<Value, rank>;
    using Data = typename PartitionedArray::Partition::Data;

    // using PartitionClient = lue::client::Partition<Index, Value, rank>;
    // using PartitionServer = typename PartitionClient::Server;
    // using Data = typename PartitionClient::Data;

    // using Definition = typename PartitionedArray::Definition;
    using Count = typename Data::Count;
    using Offset = typename PartitionedArray::Offset;
    using Shape = typename Data::Shape;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(default_construct)
{
    // Default initialization
    {
        PartitionedArray array;

        BOOST_CHECK_EQUAL(array.nr_elements(), 0);

        Shape shape{};
        BOOST_CHECK_EQUAL(array.shape(), shape);

        BOOST_CHECK_EQUAL(array.nr_partitions(), 0);
    }

    // Value initialization
    {
        PartitionedArray array{};

        BOOST_CHECK_EQUAL(array.nr_elements(), 0);

        Shape shape{};
        BOOST_CHECK_EQUAL(array.shape(), shape);

        BOOST_CHECK_EQUAL(array.nr_partitions(), 0);
    }
}


/// BOOST_AUTO_TEST_CASE(construct_with_shape)
/// {
///     Count nr_rows = 300;
///     Count nr_cols = 400;
///     Shape shape{{nr_rows, nr_cols}};
///
///     PartitionedArray array{shape};
///
///     BOOST_CHECK_EQUAL(array.nr_elements(), nr_rows * nr_cols);
///     BOOST_CHECK_EQUAL(array.shape(), shape);
///     BOOST_CHECK_EQUAL(array.nr_partitions(), hpx::get_num_localities().get());
/// }


/// BOOST_AUTO_TEST_CASE(construct_with_max_partition_shape)
/// {
///     Count nr_rows = 30;
///     Count nr_cols = 40;
///     Shape shape{{nr_rows, nr_cols}};
///
///     {
///         // 2 x 2 partitions, without clamping
///         Count nr_rows_partition = 15;
///         Count nr_cols_partition = 20;
///         Shape max_partition_shape{{nr_rows_partition, nr_cols_partition}};
///
///         PartitionedArray array{shape, max_partition_shape};
///
///         BOOST_CHECK_EQUAL(array.nr_elements(), nr_rows * nr_cols);
///         BOOST_CHECK_EQUAL(array.shape(), shape);
///         BOOST_CHECK_EQUAL(array.nr_partitions(), 4);
///     }
///
///     {
///         // 30 x 40 partitions, without clamping
///         Count nr_rows_partition = 1;
///         Count nr_cols_partition = 1;
///         Shape max_partition_shape{{nr_rows_partition, nr_cols_partition}};
///
///         PartitionedArray array{shape, max_partition_shape};
///
///         BOOST_CHECK_EQUAL(array.nr_elements(), nr_rows * nr_cols);
///         BOOST_CHECK_EQUAL(array.shape(), shape);
///         BOOST_CHECK_EQUAL(array.nr_partitions(), nr_rows * nr_cols);
///     }
///
///     {
///         // 10 x 10 partitions, without clamping
///         Count nr_rows_partition = 3;
///         Count nr_cols_partition = 4;
///         Shape max_partition_shape{{nr_rows_partition, nr_cols_partition}};
///
///         PartitionedArray array{shape, max_partition_shape};
///
///         BOOST_CHECK_EQUAL(array.nr_elements(), nr_rows * nr_cols);
///         BOOST_CHECK_EQUAL(array.shape(), shape);
///         BOOST_CHECK_EQUAL(array.nr_partitions(), 100);
///     }
///
///     {
///         // 1 x 1 partition, without clamping
///         Count nr_rows_partition = 30;
///         Count nr_cols_partition = 40;
///         Shape max_partition_shape{{nr_rows_partition, nr_cols_partition}};
///
///         PartitionedArray array{shape, max_partition_shape};
///
///         BOOST_CHECK_EQUAL(array.nr_elements(), nr_rows * nr_cols);
///         BOOST_CHECK_EQUAL(array.shape(), shape);
///         BOOST_CHECK_EQUAL(array.nr_partitions(), 1);
///     }
///
///     {
///         // 2 x 2 partitions, with clamping
///         Count nr_rows_partition = 20;
///         Count nr_cols_partition = 30;
///         Shape max_partition_shape{{nr_rows_partition, nr_cols_partition}};
///
///         PartitionedArray array{
///             shape, max_partition_shape, PartitionedArray::ClampMode::shrink};
///
///         BOOST_CHECK_EQUAL(array.nr_elements(), nr_rows * nr_cols);
///         BOOST_CHECK_EQUAL(array.shape(), shape);
///         BOOST_CHECK_EQUAL(array.nr_partitions(), 4);
///
///         auto const& partitions = array.partitions();
///         lue::wait_all(partitions);
///
///         BOOST_CHECK_EQUAL(partitions(0, 0).shape().get(), max_partition_shape);
///         BOOST_CHECK_EQUAL(partitions(0, 1).shape().get(), Shape({20, 10}));
///         BOOST_CHECK_EQUAL(partitions(1, 0).shape().get(), Shape({10, 30}));
///         BOOST_CHECK_EQUAL(partitions(1, 1).shape().get(), Shape({10, 10}));
///
///         BOOST_CHECK_EQUAL((partitions(0, 0).offset().get()), Offset({ 0,  0}));
///         BOOST_CHECK_EQUAL((partitions(0, 1).offset().get()), Offset({ 0, 30}));
///         BOOST_CHECK_EQUAL((partitions(1, 0).offset().get()), Offset({20,  0}));
///         BOOST_CHECK_EQUAL((partitions(1, 1).offset().get()), Offset({20, 30}));
///     }
///
///     {
///         // 2 x 2 partitions, with clamping
///         Count nr_rows_partition = 20;
///         Count nr_cols_partition = 30;
///         Shape max_partition_shape{{nr_rows_partition, nr_cols_partition}};
///
///         PartitionedArray array{
///             shape, max_partition_shape, PartitionedArray::ClampMode::merge};
///
///         BOOST_CHECK_EQUAL(array.nr_elements(), nr_rows * nr_cols);
///         BOOST_CHECK_EQUAL(array.shape(), shape);
///         BOOST_CHECK_EQUAL(array.nr_partitions(), 1);
///
///         auto const& partitions = array.partitions();
///         lue::wait_all(partitions);
///
///         BOOST_CHECK_EQUAL(partitions(0, 0).shape().get(), shape);
///
///         BOOST_CHECK_EQUAL((partitions(0, 0).offset().get()), Offset({ 0,  0}));
///     }
/// }


BOOST_AUTO_TEST_CASE(array_with_array_partitions)
{
    using Partitions = PartitionedArray::Partitions;
    using Partition = PartitionedArray::Partition;

    Count nr_rows = 300;
    Count nr_cols = 400;
    Shape shape_in_partitions{{nr_rows, nr_cols}};

    Partitions partitions{};
    partitions = Partitions{shape_in_partitions};

    for (auto& partition : partitions)
    {
        partition = Partition{};
    }

    std::size_t const nr_partitions = partitions.nr_elements();

    for (std::size_t i = 0; i < nr_partitions; ++i)
    {
        partitions[i] = Partition{};
    }
}
