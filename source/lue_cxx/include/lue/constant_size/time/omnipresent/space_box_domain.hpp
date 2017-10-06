#pragma once
#include "lue/constant_size/time/omnipresent/property_set.hpp"
#include "lue/constant_size/time/omnipresent/space_box.hpp"
#include "lue/constant_size/time/omnipresent/space_domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

class SpaceBoxDomain:
    public SpaceDomain
{

public:

    static hdf5::Datatype file_datatype(hdf5::Identifier const& id);

                   // SpaceBoxDomain      (lue::SpaceDomain& group);

                   // SpaceBoxDomain      (PropertySet const& property_set);

                   SpaceBoxDomain      (SpaceDomain const& space_domain);

                   SpaceBoxDomain      (SpaceDomain const& space_domain,
                                        hdf5::Datatype const& memory_datatype);

                   SpaceBoxDomain      (SpaceDomain&& space_domain,
                                        hdf5::Datatype const& memory_datatype);

                   SpaceBoxDomain      (SpaceBoxDomain const& other)=delete;

                   SpaceBoxDomain      (SpaceBoxDomain&& other)=default;

                   ~SpaceBoxDomain     ()=default;

    SpaceBoxDomain& operator=          (SpaceBoxDomain const& other)=delete;

    SpaceBoxDomain& operator=          (SpaceBoxDomain&& other)=default;

    SpaceBox const& boxes              () const;

    SpaceBox&      boxes               ();

    SpaceBox&      reserve             (hsize_t const nr_items);

private:

    SpaceBox       _boxes;

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
}  // namespace constant_size
}  // namespace lue
