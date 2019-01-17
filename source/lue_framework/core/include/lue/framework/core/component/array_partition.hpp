#pragma once
#include "lue/framework/core/component/server/array_partition.hpp"
#include "lue/framework/core/serialize/array_partition_definition.hpp"


namespace lue {
namespace client {

template<
    typename Index,
    typename Value,
    std::size_t rank>
class ArrayPartition:
    public hpx::components::client_base<
        ArrayPartition<Index, Value, rank>,
        server::ArrayPartition<Index, Value, rank>>

{

public:

    using Server = server::ArrayPartition<Index, Value, rank>;

    using Base = hpx::components::client_base<
        ArrayPartition<Index, Value, rank>,
        Server>;

    using Data = typename Server::Data;

    using Definition = typename Server::Definition;

                   ArrayPartition      ()=default;

                   ArrayPartition      (hpx::id_type const& id);

                   ArrayPartition      (hpx::future<hpx::id_type>&& id);

                   ArrayPartition      (hpx::id_type const& where,
                                        Definition const& definition);

                   ArrayPartition      (hpx::id_type const& where,
                                        Definition const& definition,
                                        Value value);

                   ArrayPartition      (ArrayPartition const&)=default;

                   ArrayPartition      (ArrayPartition&&)=default;

                   ~ArrayPartition     ()=default;

    ArrayPartition& operator=          (ArrayPartition const&)=default;

    ArrayPartition& operator=          (ArrayPartition&&)=default;

    std::shared_ptr<Server> component  () const;

private:

};

}  // namespace client
}  // namespace lue
