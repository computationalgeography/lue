#pragma once
#include "lue/cxx_api/constant_size/time/omnipresent/space_domain.h"
#include "lue/cxx_api/constant_size/time/omnipresent/space_box.h"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

/*!
    @ingroup    lue_cxx_api_group
*/
class SpaceBoxDomain:
    public SpaceDomain
{

public:

                   SpaceBoxDomain      (lue::SpaceDomain& group);

                   SpaceBoxDomain      (lue::SpaceDomain& group,
                                        hid_t const type_id);

                   SpaceBoxDomain      (SpaceBoxDomain const& other)=delete;

                   SpaceBoxDomain      (SpaceBoxDomain&& other)=default;

                   ~SpaceBoxDomain     ()=default;

    SpaceBoxDomain& operator=          (SpaceBoxDomain const& other)=delete;

    SpaceBoxDomain& operator=          (SpaceBoxDomain&& other)=default;

    SpaceBox&       boxes              ();

    SpaceBox&       reserve_items      (hsize_t const nr_items);

private:

    SpaceBox       _boxes;

};


void               configure_space_box_domain(
                                        hdf5::Identifier const& location,
                                        hid_t const file_type_id,
                                        hid_t const memory_type_id,
                                        size_t rank);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
