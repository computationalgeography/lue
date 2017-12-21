#pragma once
#include "lue/item/constant_size/variable/constant_shape/same_shape/collection.hpp"
#include "lue/constant_size/time/located/shared/property_set.hpp"
#include "lue/constant_size/time/located/shared/space_domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

class SpaceBoxDomain:
    public SpaceDomain
{

public:

    static hdf5::Datatype file_datatype(hdf5::Group const& parent);

    using SpaceBoxes = variable::constant_shape::same_shape::Collection;

                   SpaceBoxDomain      (SpaceDomain&& space_domain);

                   SpaceBoxDomain      (SpaceDomain&& space_domain,
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
                                        size_t rank);

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
