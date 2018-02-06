#pragma once
#include "lue/space/constant_collection/stationary/point.hpp"
#include "lue/constant_collection/property_set.hpp"
#include "lue/constant_collection/space_domain.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {

class SpacePointDomain:
    public constant_collection::SpaceDomain
{

public:

    static hdf5::Datatype file_datatype(hdf5::Group const& parent);

    using SpacePoints = stationary::Point;

                   SpacePointDomain    (hdf5::Group const& parent);

                   SpacePointDomain    (hdf5::Group const& parent,
                                        hdf5::Datatype const& memory_datatype);

                   SpacePointDomain    (constant_collection::SpaceDomain&&
                                            space_domain);

                   SpacePointDomain    (constant_collection::SpaceDomain&&
                                            space_domain,
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
                                        std::size_t rank);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
