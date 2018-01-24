#pragma once
#include "lue/item/constant_collection/constant_shape/same_shape/constant.hpp"
#include "lue/constant_collection/time/omnipresent/property_set.hpp"
#include "lue/constant_collection/time/omnipresent/space_domain.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {

class SpacePointDomain:
    public SpaceDomain
{

public:

    static hdf5::Datatype file_datatype(hdf5::Group const& parent);

    using SpacePoints = constant_shape::same_shape::Constant;

                   SpacePointDomain    (SpaceDomain&& space_domain);

                   SpacePointDomain    (SpaceDomain&& space_domain,
                                        hdf5::Datatype const& memory_datatype);

                   SpacePointDomain    (SpacePointDomain const&)=delete;

                   SpacePointDomain    (SpacePointDomain&&)=default;

                   ~SpacePointDomain   ()=default;

    SpacePointDomain& operator=        (SpacePointDomain const&)=delete;

    SpacePointDomain& operator=        (SpacePointDomain&&)=default;

    SpacePoints const& items           () const;

    SpacePoints&   items               ();

    SpacePoints&   reserve             (hsize_t nr_items);

private:

    SpacePoints       _items;

};


SpacePointDomain     create_space_point_domain(
                                        PropertySet& property_set,
                                        hdf5::Datatype const file_datatype,
                                        hdf5::Datatype const memory_datatype,
                                        size_t const rank);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
