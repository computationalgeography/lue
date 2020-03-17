#include "lue/framework/io/write.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {

template<
    typename Array,
    typename DatasetPtr>
hpx::future<void> write(
    Array const& array,
    typename data_model::constant::RasterView<DatasetPtr>::Layer& layer)
{
    // TODO Make this asynchronous

    // Asynchronously write the partitions to the raster layer

    using PartitionData = DataT<Array>;
    using Offset = OffsetT<Array>;

    // -------------------------------------------------------------------------
    // Once all partitions are finished being calculated, ask for the
    // partition data
    hpx::wait_all_n(array.begin(), array.nr_partitions());

    auto const nr_partitions{lue::nr_partitions(array)};

    std::vector<hpx::future<PartitionData>> partition_datas(nr_partitions);
    std::vector<hpx::future<Offset>> partition_offsets(nr_partitions);

    for(Index p = 0; p < nr_partitions; ++p) {
        partition_datas[p] = array.partitions()[p].data();
        partition_offsets[p] = array.partitions()[p].offset();
    }

    hpx::wait_all_n(partition_datas.begin(), partition_datas.size());
    hpx::wait_all_n(partition_offsets.begin(), partition_offsets.size());

    // Shape of object array: (nr_rows, nr_cols)
    assert(layer.shape().size() == 2);
    hdf5::Offset offset(layer.shape().size());
    hdf5::Count count(layer.shape().size());

    for(Index p = 0; p < nr_partitions; ++p) {

        auto const& partition_data{partition_datas[p].get()};
        auto const& partition_offset{partition_offsets[p].get()};
        auto const& partition_shape{partition_data.shape()};

        // Location of partition in HDF5 dataset
        std::copy(
            partition_offset.begin(), partition_offset.end(), offset.begin());

        // Extent of partition in HDF5 dataset
        std::copy(
            partition_shape.begin(), partition_shape.end(), count.begin());

        hdf5::Hyperslab hyperslab{offset, count};

        layer.write(hyperslab, &(*partition_data.begin()));

    }

    return hpx::make_ready_future<void>();
}


template<
    typename Array,
    typename DatasetPtr>
hpx::future<void> write(
    Array const& array,
    typename data_model::variable::RasterView<DatasetPtr>::Layer& layer,
    data_model::Index const idx)
{
    // TODO Make this asynchronous

    // FIXME
    // - Collective I/O?
    // - One process per locality (node?)

    // Asynchronously write the partitions to the raster layer

    // auto const& shape{array.shape()};

    // Iterate over all partitions

        // Ask for the data

    // Once the data arrived, write it to the layer

    // using Partitions = PartitionsT<Array>;
    // using Partition = PartitionT<Array>;
    using PartitionData = DataT<Array>;
    // using Element = ElementT<Array>;
    using Offset = OffsetT<Array>;

    // -------------------------------------------------------------------------
    // Once all partitions are finished being calculated, ask for the
    // partition data
    hpx::wait_all_n(array.begin(), array.nr_partitions());

    auto const nr_partitions{lue::nr_partitions(array)};

    std::vector<hpx::future<PartitionData>> partition_datas(nr_partitions);
    std::vector<hpx::future<Offset>> partition_offsets(nr_partitions);

    for(Index p = 0; p < nr_partitions; ++p) {
        partition_datas[p] = array.partitions()[p].data();
        partition_offsets[p] = array.partitions()[p].offset();
    }

    hpx::wait_all_n(partition_datas.begin(), partition_datas.size());
    hpx::wait_all_n(partition_offsets.begin(), partition_offsets.size());

    // Shape of object array: (nr_time_steps, nr_rows, nr_cols)
    assert(layer.rank() == 3);
    hdf5::Offset offset(layer.rank() + 1);
    hdf5::Count count(layer.rank() + 1);

    // Jump to the one and only location in time (discretized time box)
    offset[0] = 0;

    // Jump to time step passed in
    offset[1] = idx;

    // Select all locations in time (the single time box)
    count[0] = 1;

    // Select a single time step from the discretized time box
    count[1] = 1;

    for(Index p = 0; p < nr_partitions; ++p) {

        auto const& partition_data{partition_datas[p].get()};
        auto const& partition_offset{partition_offsets[p].get()};
        auto const& partition_shape{partition_data.shape()};

        // Location of partition in HDF5 dataset
        std::copy(
            partition_offset.begin(), partition_offset.end(),
            offset.begin() + 2);

        // Extent of partition in HDF5 dataset
        std::copy(
            partition_shape.begin(), partition_shape.end(),
            count.begin() + 2);

        hdf5::Hyperslab hyperslab{offset, count};

        layer.write(hyperslab, &(*partition_data.begin()));

    }

    return hpx::make_ready_future<void>();
}


template<
    typename Array>
hpx::future<void> write(
    Array const& /* array */,
    std::string const& /* pathname */)
{
    // TODO Create and use constant::RasterView

    assert(false);

    return hpx::make_ready_future<void>();
}


#define INSTANTIATE_WRITE_1(Element, DatasetPtr)                             \
template hpx::future<void> write<PartitionedArray<Element, 2>, DatasetPtr>(  \
    PartitionedArray<Element, 2> const& array,                               \
    data_model::constant::RasterView<DatasetPtr>::Layer& layer);             \
                                                                             \
template hpx::future<void> write<PartitionedArray<Element, 2>, DatasetPtr>(  \
    PartitionedArray<Element, 2> const& array,                               \
    data_model::variable::RasterView<DatasetPtr>::Layer& layer,              \
    data_model::Index idx);

#define INSTANTIATE_WRITE_2(Element)                             \
template hpx::future<void> write<PartitionedArray<Element, 2>>(  \
    PartitionedArray<Element, 2> const& array,                   \
    std::string const& pathname);

#define INSTANTIATE_WRITE(Element)                                      \
    INSTANTIATE_WRITE_1(Element, data_model::Dataset*)                  \
    INSTANTIATE_WRITE_1(Element, std::shared_ptr<data_model::Dataset>)  \
    INSTANTIATE_WRITE_2(Element)

INSTANTIATE_WRITE(uint32_t)
INSTANTIATE_WRITE(int32_t)
INSTANTIATE_WRITE(double)

#undef INSTANTIATE_WRITE
#undef INSTANTIATE_WRITE_2
#undef INSTANTIATE_WRITE_1

}  // namespace lue
