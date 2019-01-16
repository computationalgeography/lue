#pragma once
#include "lue/framework/core/partition_definition.hpp"
#include <boost/multi_array.hpp>


namespace lue {

template<
    typename Index,
    typename Value,
    std::size_t rank>
class PartitionData
{

public:

    using Definition = PartitionDefinition<Index, rank>;

    using Values = boost::multi_array<Value, rank>;

                   PartitionData       ();

                   PartitionData       (Definition const& definition);

                   PartitionData       (Definition const& definition,
                                        Value const& value);

                   PartitionData       (PartitionData const&)=default;

                   PartitionData       (PartitionData&&)=default;

                   ~PartitionData      ()=default;

    PartitionData& operator=           (PartitionData const&)=default;

    PartitionData& operator=           (PartitionData&&)=default;

    bool           operator==          (PartitionData const& other) const;

    Definition const& definition       () const;

    Values const&  values              () const;

    Values&        values              ();

    // Value              value               (std::size_t const row_idx,
    //                                     std::size_t const col_idx) const;

    // void           set_value           (std::size_t const row_idx,
    //                                     std::size_t const col_idx,
    //                                     Value const& value);

private:

    Definition     _definition;

    Values         _values;

};

}  // namespace lue
