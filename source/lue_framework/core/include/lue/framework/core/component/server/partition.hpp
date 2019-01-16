#pragma once
#include <hpx/config.hpp>
#include "lue/framework/core/partition_data.hpp"
#include <hpx/include/components.hpp>


namespace lue {
namespace server {

template<
    typename Index,
    typename Value,
    std::size_t rank>
class Partition:
    public hpx::components::component_base<
        Partition<Index, Value, rank>>

    // KDJ: Thread safe version. Not sure if we need this.
    // public hpx::components::locking_hook<
    //     hpx::components::component_base<Partition>>

{

public:

    using Data = PartitionData<Index, Value, rank>;

    using Definition = PartitionDefinition<Index, rank>;

                   Partition           ()=default;

                   Partition           (Definition const& definition);

                   Partition           (Definition const& definition,
                                        Value value);

                   Partition           (Partition const&)=default;

                   Partition           (Partition&&)=default;

                   ~Partition          ()=default;

    Partition&     operator=           (Partition const&)=default;

    Partition&     operator=           (Partition&&)=default;

    Data const&    data                () const;

private:

    Data           _data;

};

}  // namespace server
}  // namespace lue
