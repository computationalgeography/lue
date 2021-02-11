#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/hdf5/hyperslab.hpp"
#include <boost/algorithm/string/split.hpp>
#include <filesystem>
#include <map>


namespace lue {
namespace detail {

hpx::future<void> when_all_get(
    std::vector<hpx::future<void>>&& futures)
{
    // When waiting on the result of asynchronous actions returning
    // nothing (hpx::future<void>), any exception thrown will evaporate if
    // we don't get the 'value'. Here we 'wait' for all futures to become
    // ready, and then get the 'value' to let any exception be rethrown.
    // The caller, in turn, has to get the 'value' of the future we return!
    return hpx::when_all(futures.begin(), futures.end()).then(

            [](hpx::future<std::vector<hpx::future<void>>> futures_ftr)
            {
                auto futures{futures_ftr.get()};
                std::for_each(futures.begin(), futures.end(), [](hpx::future<void>& future) { future.get(); });
            }

        );
}

template<
    typename Shape>
Shape hyperslab_to_shape(
    hdf5::Hyperslab const& hyperslab)
{
    // Return the shape represented by the hyperslab passed in
    lue_assert(std::all_of(hyperslab.stride().begin(), hyperslab.stride().end(),
        [](auto const s) { return s == 1; }));
    auto const& count{hyperslab.count()};
    Shape shape;

    lue_assert(count.size() == shape.size());
    std::copy(count.begin(), count.end(), shape.begin());

    return shape;
}


template<
    Rank rank>
hdf5::Hyperslab hyperslab(
    Offset<Index, rank> const& partition_offset,
    Shape<Count, rank> const& partition_shape)
{
    hdf5::Offset offset(rank);
    std::copy(partition_offset.begin(), partition_offset.end(), offset.begin());

    hdf5::Count count(rank);
    std::copy(partition_shape.begin(), partition_shape.end(), count.begin());

    return hdf5::Hyperslab{offset, count};
}


template<
    Rank rank>
hdf5::Hyperslab hyperslab(
    Offset<Index, rank> const& partition_offset,
    Shape<Count, rank> const& partition_shape,
    Index const location_in_time_idx,
    Index const time_step_idx)
{
    hdf5::Offset offset(rank + 2);
    std::copy(partition_offset.begin(), partition_offset.end(), offset.begin() + 2);
    offset[0] = location_in_time_idx;
    offset[1] = time_step_idx;

    hdf5::Count count(rank + 2);
    std::copy(partition_shape.begin(), partition_shape.end(), count.begin() + 2);
    count[0] = 1;
    count[1] = 1;

    return hdf5::Hyperslab{offset, count};
}


template<
    typename PartitionServer>
hdf5::Hyperslab hyperslab(
    PartitionServer const& partition_server)
{
    // Return the part of the array represented by the partition server
    // instance passed in, as an HDF5 hyperslab. The result is useful
    // for performing I/O to the corresponding HDF5 dataset.
    return hyperslab(partition_server.offset(), partition_server.shape());
}


template<
    typename PartitionServer>
hdf5::Hyperslab hyperslab(
    PartitionServer const& partition_server,
    Index const location_in_time_idx,
    Index const time_step_idx)
{
    // Return the part of the array represented by the partition server
    // instance passed in, as an HDF5 hyperslab. The result is useful
    // for performing I/O to the corresponding HDF5 dataset.
    return hyperslab(
        partition_server.offset(), partition_server.shape(), location_in_time_idx, time_step_idx);
}


template<
    typename ComponentClient>
std::shared_ptr<typename ComponentClient::Server> ready_component_ptr(
    ComponentClient& client)
{
    // Only call this function on the same locality as the component
    // Only call this function when the client is ready
    lue_assert(client.is_ready());

    using ComponentServer = typename ComponentClient::Server;

    hpx::future<std::shared_ptr<ComponentServer>> server_ptr_ftr{
        hpx::get_ptr<ComponentServer>(client.get_id())};
    lue_assert(server_ptr_ftr.is_ready());

    return server_ptr_ftr.get();
}


std::array<std::string, 4> parse_array_pathname(
        std::string const& array_pathname)
{
    // Parse array pathname into
    // <dataset_pathname>/<phenomenon_name>/<property_set_name>/<property_name>
    std::string dataset_pathname, phenomenon_name, property_set_name, property_name;

    {
        // Split on forward slash and grab the elements from end. The
        // remaining ones belong to the dataset pathname.
        std::vector<std::string> tokens;
        boost::algorithm::split(tokens, array_pathname, [](char const c) { return c == '/'; });

        if(tokens.size() < 4)
        {
            throw std::runtime_error(fmt::format(
                "Array pathname must be formatted as "
                "<dataset_pathname>/<phenomenon_name>/<property_set_name>/<property_name>; got {}.",
                array_pathname));
        }

        property_name = tokens.back(); tokens.pop_back();
        property_set_name = tokens.back(); tokens.pop_back();
        phenomenon_name = tokens.back(); tokens.pop_back();

        std::filesystem::path dataset_path{tokens.front()};

        for(auto it = tokens.begin() + 1; it != tokens.end(); ++it)
        {
            dataset_path /= *it;
        }

        dataset_pathname = dataset_path;

        if(std::filesystem::path{array_pathname}.is_absolute())
        {
            dataset_pathname = "/" + dataset_pathname;
        }
    }

    return std::array<std::string, 4>{dataset_pathname, phenomenon_name, property_set_name, property_name};
}


template<
    typename Element,
    Rank rank>
std::map<hpx::id_type, std::vector<PartitionT<PartitionedArray<Element, rank>>>> partitions_by_locality(
    PartitionedArray<Element, rank> const& array)
{
    // The partitions in the array are distributed over localities. We
    // perform I/O per locality, so for each of the localities, we must
    // determine which partitions are stored there. Then we can attach
    // a continuation to the group of partitions that will perform the read
    // on the locality.

    using Array = PartitionedArray<Element, rank>;
    using Partitions = PartitionsT<Array>;
    using Partition = PartitionT<Array>;

    std::map<hpx::id_type, std::vector<Partition>> result;

    {
        Localities<rank> const& localities{array.localities()};
        Partitions const& partitions{array.partitions()};
        auto const nr_partitions{lue::nr_partitions(array)};

        {
            // Try to prevent reallocations while adding partitions to the
            // collections. Assume the partitions are evenly distributed
            // over all localities.
            Count const nr_partitions_per_locality{
                (nr_partitions / Count(hpx::find_all_localities().size())) + 1};

            for(auto& [locality, partitions]: result)
            {
                partitions.reserve(nr_partitions_per_locality);
            }
        }

        for(Index p = 0; p < nr_partitions; ++p)
        {
            // TODO
            // lue_assert(result[localities[p]].capacity() > result[localities[p]].size());
            result[localities[p]].push_back(partitions[p]);
        }
    }

    return result;
}

}  // namespace detail
}  // namespace lue
