#include "lue/cxx_api/time/omnipresent/space_box_domain.h"


namespace lue {
namespace time {
namespace omnipresent {

SpaceBoxDomain::SpaceBoxDomain(
    lue::SpaceDomain& group)

    : SpaceDomain(group),
      _boxes(group.id())

{
}


SpaceBoxDomain::SpaceBoxDomain(
    lue::SpaceDomain& group,
    hid_t const type_id)

    : SpaceDomain(group),
      _boxes(group.id(), type_id)

{
}


SpaceBox& SpaceBoxDomain::boxes()
{
    return _boxes;
}


SpaceBox::Item& SpaceBoxDomain::reserve_items(
    hsize_t const nr_items)
{
    _boxes.reserve_items(nr_items);

    return _boxes;
}


void configure_space_box_domain(
    hdf5::Identifier const& location,
    hid_t const file_type_id,
    hid_t const memory_type_id,
    size_t rank)
{
    create_space_box(location, file_type_id, memory_type_id, rank);
}

}  // namespace omnipresent
}  // namespace time
}  // namespace lue
