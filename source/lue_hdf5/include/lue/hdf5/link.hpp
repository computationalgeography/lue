#pragma once
#include "lue/hdf5/identifier.hpp"


namespace lue {
namespace hdf5 {

class Group;


class Link
{

public:

                   Link                (Group& group,
                                        std::string const& name);

                   Link                (Group const&)=delete;

                   Link                (Link&&)=default;

                   ~Link               ()=default;

    Link&          operator=           (Link const&)=delete;

    Link&          operator=           (Link&&)=default;

    Identifier const& location_id      () const;

    std::string const& name            () const;

protected:

    std::size_t    value_size          () const;

private:

    ::H5L_info_t   info                () const;

    Identifier     _location_id;

    std::string const _name;

};


::H5O_type_t       object_type         (Identifier const& id,
                                        std::string const& name);

::H5L_info_t       link_info           (Identifier const& id,
                                        std::string const& name);

::H5L_type_t       link_type           (Identifier const& id,
                                        std::string const& name);

std::size_t        link_value_size     (Identifier const& id,
                                        std::string const& name);

bool               link_exists         (Identifier const& id,
                                        std::string const& name);

bool               hard_link_exists    (Identifier const& id,
                                        std::string const& name);

bool               link_is_dataset     (Identifier const& id,
                                        std::string const& name);

bool               link_is_group       (Identifier const& id,
                                        std::string const& name);

}  // namespace hdf5
}  // namespace lue
