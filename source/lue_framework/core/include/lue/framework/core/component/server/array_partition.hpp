#pragma once
#include <hpx/config.hpp>
#include "lue/framework/core/array_partition_data.hpp"
#include <hpx/include/components.hpp>


namespace lue {
namespace server {

template<
    typename Index,
    typename Value,
    std::size_t rank>
class ArrayPartition:
    public hpx::components::component_base<
        ArrayPartition<Index, Value, rank>>

    // KDJ: Thread safe version. Not sure if we need this.
    // public hpx::components::locking_hook<
    //     hpx::components::component_base<ArrayPartition>>

{

public:

    using Data = ArrayPartitionData<Index, Value, rank>;

    using Definition = PartitionDefinition<Index, rank>;

                   ArrayPartition      ()=default;

                   ArrayPartition      (Definition const& definition);

                   ArrayPartition      (Definition const& definition,
                                        Value value);

                   ArrayPartition      (ArrayPartition const&)=default;

                   ArrayPartition      (ArrayPartition&&)=default;

                   ~ArrayPartition     ()=default;

    ArrayPartition& operator=          (ArrayPartition const&)=default;

    ArrayPartition& operator=          (ArrayPartition&&)=default;

    Data const&    data                () const;

private:

    Data           _data;

};

}  // namespace server
}  // namespace lue
