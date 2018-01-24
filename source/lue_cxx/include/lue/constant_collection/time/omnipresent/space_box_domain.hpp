#pragma once
#include "lue/item/constant_collection/constant_shape/same_shape/constant.hpp"
#include "lue/constant_collection/time/omnipresent/property_set.hpp"
#include "lue/constant_collection/time/omnipresent/space_domain.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {

class SpaceBoxDomain:
    public SpaceDomain
{

public:

    static hdf5::Datatype file_datatype(hdf5::Group const& parent);

    using SpaceBoxes = constant_shape::same_shape::Constant;

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


// bool               space_box_domain_exists(
//                                         hdf5::Group const& group);
//                                         // PropertySet const& property_set);

SpaceBoxDomain     create_space_box_domain(
                                        PropertySet& property_set,
                                        hdf5::Datatype const file_datatype,
                                        hdf5::Datatype const memory_datatype,
                                        size_t const rank);


// void               create_space_box_domain(
//                                         hdf5::Identifier const& location,
//                                         hid_t const file_type_id,
//                                         hid_t const memory_type_id,
//                                         size_t rank);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
