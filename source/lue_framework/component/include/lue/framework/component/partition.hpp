#pragma once
#include "lue/framework/component/server/partition.hpp"
#include "lue/framework/core/serialize/partition_definition.hpp"


namespace lue {
namespace client {

template<
    typename Index,
    typename Value,
    std::size_t rank>
class Partition:
    public hpx::components::client_base<
        Partition<Index, Value, rank>,
        server::Partition<Index, Value, rank>>

{

public:

    using Server = server::Partition<Index, Value, rank>;

    using Base = hpx::components::client_base<
        Partition<Index, Value, rank>,
        Server>;

    using Data = typename Server::Data;

    using Definition = typename Server::Definition;

                   Partition           ()=default;

                   Partition           (hpx::id_type const& id);

                   Partition           (hpx::future<hpx::id_type>&& id);

                   Partition           (hpx::id_type const& where,
                                        Definition const& definition);

                   Partition           (hpx::id_type const& where,
                                        Definition const& definition,
                                        Value value);

                   Partition           (Partition const&)=default;

                   Partition           (Partition&&)=default;

                   ~Partition          ()=default;

    Partition&     operator=           (Partition const&)=default;

    Partition&     operator=           (Partition&&)=default;

    std::shared_ptr<Server> component  () const;

private:

};

}  // namespace client
}  // namespace lue
