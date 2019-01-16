#pragma once
#include "lue/framework/core/partition_definition.hpp"
#include <boost/multi_array.hpp>


namespace lue {

template<
    typename Index,
    typename Value,
    std::size_t rank>
class ArrayPartitionData
{

public:

    using Definition = PartitionDefinition<Index, rank>;

    using Values = boost::multi_array<Value, rank>;

                   ArrayPartitionData  ();

                   ArrayPartitionData  (Definition const& definition);

                   ArrayPartitionData  (Definition const& definition,
                                        Value const& value);

                   ArrayPartitionData  (ArrayPartitionData const&)=default;

                   ArrayPartitionData  (ArrayPartitionData&&)=default;

                   ~ArrayPartitionData ()=default;

    ArrayPartitionData& operator=      (ArrayPartitionData const&)=default;

    ArrayPartitionData& operator=      (ArrayPartitionData&&)=default;

    bool           operator==          (ArrayPartitionData const& other) const;

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
