#include "lue/constant_size/time/omnipresent/space_box_domain.h"
#include "lue/tag.h"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

hdf5::Datatype SpaceBoxDomain::file_datatype(
    hdf5::Identifier const& id)
{
    return hdf5::Dataset(id, coordinates_tag).datatype();
}


// SpaceBoxDomain::SpaceBoxDomain(
//     lue::SpaceDomain& group)
// 
//     : SpaceDomain(group),
//       _boxes(group.id())
// 
// {
// }


SpaceBoxDomain::SpaceBoxDomain(
    SpaceDomain const& space_domain,
    hdf5::Datatype const& memory_datatype)

    : SpaceDomain(space_domain),
      _boxes(id(), memory_datatype)

{
}


SpaceBoxDomain::SpaceBoxDomain(
    SpaceDomain&& space_domain,
    hdf5::Datatype const& memory_datatype)

    : SpaceDomain(std::forward<SpaceDomain>(space_domain)),
      _boxes(id(), memory_datatype)

{
}


SpaceBox const& SpaceBoxDomain::boxes() const
{
    return _boxes;
}


SpaceBox& SpaceBoxDomain::boxes()
{
    return _boxes;
}


SpaceBox& SpaceBoxDomain::reserve(
    hsize_t const nr_items)
{
    _boxes.reserve(nr_items);

    return _boxes;
}


// void configure_space_box_domain(
//     hdf5::Identifier const& location,
//     hid_t const file_type_id,
//     hid_t const memory_type_id,
//     size_t rank)
// {
//     create_space_box(location, file_type_id, memory_type_id, rank);
// }


SpaceBoxDomain configure_space_box_domain(
    PropertySet& property_set,
    hdf5::Datatype const file_datatype,
    hdf5::Datatype const memory_datatype,
    size_t const rank)
{
    auto& domain = property_set.domain();
    auto& space = domain.space();

    create_space_box(space, file_datatype, memory_datatype, rank);

    return SpaceBoxDomain(space, memory_datatype);
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
