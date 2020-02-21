#include "lue/framework/io/write.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {

template<
    typename Array>
hpx::future<void> write(
    Array const& array,
    data_model::variable::RasterView::Layer& layer,
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
    using Element = ElementT<Array>;
    using Offset = OffsetT<Array>;

    // -------------------------------------------------------------------------
    // Once all partitions are finished being calculated, ask for the
    // partition data
    hpx::wait_all_n(array.begin(), array.nr_partitions());

    auto const nr_partitions{lue::nr_partitions(array)};

    std::vector<hpx::future<PartitionData>> partition_datas(nr_partitions);
    std::vector<hpx::future<Offset>> partition_offsets(nr_partitions);

    for(Index p = 0; p < nr_partitions; ++p) {
        partition_datas[p] = array.partitions()[p].data(CopyMode::share);
        partition_offsets[p] = array.partitions()[p].offset();
    }

    hpx::wait_all_n(partition_datas.begin(), partition_datas.size());
    hpx::wait_all_n(partition_offsets.begin(), partition_offsets.size());

    assert(layer.rank() == 2);  // Rank of object array
    hdf5::Offset offset(layer.rank() + 1);
    hdf5::Count count(layer.rank() + 1);

    offset[0] = idx;
    count[0] = 1;

    for(Index p = 0; p < nr_partitions; ++p) {

        auto const& partition_data{partition_datas[p].get()};
        auto const& partition_offset{partition_offsets[p].get()};
        auto const& partition_shape{partition_data.shape()};

        // Location of partition in HDF5 dataset
        std::copy(
            partition_offset.begin(), partition_offset.end(),
            offset.begin() + 1);

        // Extent of partition in HDF5 dataset
        std::copy(
            partition_shape.begin(), partition_shape.end(),
            count.begin() + 1);

        hdf5::Hyperslab hyperslab{offset, count};

        layer.write(hyperslab, partition_data.begin());

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


#define INSTANTIATE_WRITE(Element)                               \
                                                                 \
template hpx::future<void> write<PartitionedArray<Element, 2>>(  \
    PartitionedArray<Element, 2> const& array,                   \
    data_model::variable::RasterView::Layer& layer,              \
    data_model::Index idx);                                      \
                                                                 \
template hpx::future<void> write<PartitionedArray<Element, 2>>(  \
    PartitionedArray<Element, 2> const& array,                   \
    std::string const& pathname);

INSTANTIATE_WRITE(int32_t)
INSTANTIATE_WRITE(double)

#undef INSTANTIATE_WRITE

}  // namespace lue
