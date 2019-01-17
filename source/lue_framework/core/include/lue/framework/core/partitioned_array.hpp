#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include "lue/framework/core/spatial_index.hpp"


namespace lue {

/*!
    @brief      This class represents a collection of partitions which
                are distributed over a number of localities

    The partitions are stored in a spatial index.
*/
template<
    typename Index,
    typename Value,
    std::size_t rank>
class PartitionedArray
{

public:

    using PartitionClient = client::ArrayPartition<Index, Value, rank>;

    using PartitionServer = typename PartitionClient::Server;

    using Partitions = ArrayPartitionData<Index, PartitionClient, rank>;

    using Definition = ArrayPartitionDefinition<Index, rank>;

    using Shape = typename Definition::Shape;



    // using Value = std::tuple<
    //     Envelope<double, 2>,
    //     hpx::id_type,
    //     client::ArrayPartition<T, 2>>;

    // using Index = SpatialIndex<Value>;

    // using const_iterator = typename Index::const_iterator;

    // static auto const& envelope(Value const& v) { return std::get<0>(v); }

    // static auto locality(Value const& v) { return std::get<1>(v); }

    // static auto const& partition(Value const& v) { return std::get<2>(v); }

                   PartitionedArray    ()=default;

                   PartitionedArray    (Definition const& definition);

    // template<
    //     typename DistributionPolicy>
    //                PartitionedArray    (Definition const& definition,
    //                                     DistributionPolicy const& distribution_policy);

    //                PartitionedArray    (Definition const& definition,
    //                                     Envelope<double, 2> const& envelope);

    //                PartitionedArray    (PartitionedArray const& other)=default;

    //                PartitionedArray    (PartitionedArray&& other)=default;

    //                ~PartitionedArray   ()=default;

    // PartitionedArray& operator=        (PartitionedArray const& other)=default;

    // PartitionedArray& operator=        (PartitionedArray&& other)=default;

    // void           insert              (Value const& value);

    Index             nr_elements      () const;

    Definition const& definition       () const;

    Index             nr_partitions    () const;

    Partitions const& partitions       () const;

    // Envelope<double, 2> const&
    //                envelope            () const;

    // // void           wait                ();

    // bool           valid               () const;

    // const_iterator begin               () const;

    // const_iterator end                 () const;

private:

    //! Per locality ID a list of array partition component IDs
    using BulkLocalityResult =
        std::pair<hpx::id_type, std::vector<hpx::id_type>>;

    template<
        typename DistributionPolicy,
        typename Creator>
    void           create              (DistributionPolicy const&
                                            distribution_policy,
                                        Creator&& creator);

    template<
        typename DistributionPolicy>
    void           create              (DistributionPolicy const&
                                            distribution_policy);

    //! Definition of the array
    Definition     _definition;

    //! Array of array partitions
    Partitions     _partitions;

    // Envelope<double, 2> _envelope;

    // Index          _index;

};

}  // namespace lue
