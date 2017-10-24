#pragma once
#include "lue/constant_size/time/omnipresent/property_set.hpp"
#include "lue/constant_size/time/omnipresent/space_point.hpp"
#include "lue/constant_size/time/omnipresent/space_domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

class SpacePointDomain:
    public SpaceDomain
{

public:

    static hdf5::Datatype file_datatype(hdf5::Identifier const& id);

                   SpacePointDomain    (SpaceDomain&& space_domain);

                   SpacePointDomain    (SpaceDomain&& space_domain,
                                        hdf5::Datatype const& memory_datatype);

                   SpacePointDomain    (SpacePointDomain const& other)=delete;

                   SpacePointDomain    (SpacePointDomain&& other)=default;

                   ~SpacePointDomain   ()=default;

    SpacePointDomain& operator=        (SpacePointDomain const& other)=delete;

    SpacePointDomain& operator=        (SpacePointDomain&& other)=default;

    SpacePoint const& items            () const;

    SpacePoint&      items             ();

    SpacePoint&      reserve           (hsize_t const nr_items);

private:

    SpacePoint       _items;

};


SpacePointDomain     create_space_point_domain(
                                        PropertySet& property_set,
                                        hdf5::Datatype const file_datatype,
                                        hdf5::Datatype const memory_datatype,
                                        size_t const rank);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
