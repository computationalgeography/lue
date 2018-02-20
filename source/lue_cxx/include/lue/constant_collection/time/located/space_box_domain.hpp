#pragma once
#include "lue/space/stationary/box.hpp"
#include "lue/constant_collection/property_set.hpp"
#include "lue/constant_collection/space_domain.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace located {

class SpaceBoxDomain:
    public constant_collection::SpaceDomain
{

public:

    static hdf5::Datatype file_datatype(hdf5::Group const& parent);

    using SpaceBoxes = stationary::Box;

                   SpaceBoxDomain      (hdf5::Group const& parent);

                   SpaceBoxDomain      (hdf5::Group const& parent,
                                        hdf5::Datatype const& memory_datatype);

                   SpaceBoxDomain      (constant_collection::SpaceDomain&&
                                            space_domain);

                   SpaceBoxDomain      (constant_collection::SpaceDomain&&
                                            space_domain,
                                        hdf5::Datatype const& memory_datatype);

                   SpaceBoxDomain      (SpaceBoxDomain const&)=delete;

                   SpaceBoxDomain      (SpaceBoxDomain&&)=default;

                   ~SpaceBoxDomain     ()=default;

    SpaceBoxDomain& operator=          (SpaceBoxDomain const&)=delete;

    SpaceBoxDomain& operator=          (SpaceBoxDomain&&)=default;

    SpaceBoxes const& items            () const;

    SpaceBoxes&    items               ();

    SpaceBoxes&    reserve             (hsize_t nr_items);

private:

    SpaceBoxes     _items;

};


SpaceBoxDomain     create_space_box_domain(
                                        PropertySet& property_set,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
