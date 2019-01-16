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
    typename T>
class PartitionedArray
{

public:

    // using Value = std::tuple<
    //     Envelope<double, 2>,
    //     hpx::id_type,
    //     client::ArrayPartition<T, 2>>;

    // using Index = SpatialIndex<Value>;

    // using const_iterator = typename Index::const_iterator;

    // using Definition = PartitionDefinition<2>;

    // static auto const& envelope(Value const& v) { return std::get<0>(v); }

    // static auto locality(Value const& v) { return std::get<1>(v); }

    // static auto const& partition(Value const& v) { return std::get<2>(v); }

    //                PartitionedArray    (Definition const& definition,
    //                                     Envelope<double, 2> const& envelope);

    //                PartitionedArray    (PartitionedArray const& other)=default;

    //                PartitionedArray    (PartitionedArray&& other)=default;

    //                ~PartitionedArray   ()=default;

    // PartitionedArray& operator=        (PartitionedArray const& other)=default;

    // PartitionedArray& operator=        (PartitionedArray&& other)=default;

    // void           insert              (Value const& value);

    // bool           empty               () const;

    // std::size_t    size                () const;

    // Definition const& definition       () const;

    // Envelope<double, 2> const&
    //                envelope            () const;

    // // void           wait                ();

    // bool           valid               () const;

    // const_iterator begin               () const;

    // const_iterator end                 () const;

private:

    // Definition     _definition;

    // Envelope<double, 2> _envelope;

    // Index          _index;

};

}  // namespace lue
