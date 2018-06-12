#include "lue/constant_collection/time/located/space_box_domain.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace located {

hdf5::Datatype SpaceBoxDomain::file_datatype(
    hdf5::Group const& parent)
{
    return hdf5::Dataset(parent, coordinates_tag).datatype();
}


SpaceBoxDomain::SpaceBoxDomain(
    hdf5::Group const& parent)

    : SpaceBoxDomain{parent, hdf5::memory_datatype(file_datatype(parent))}

{
}


SpaceBoxDomain::SpaceBoxDomain(
    hdf5::Group const& parent,
    hdf5::Datatype const& memory_datatype)

    : constant_collection::SpaceDomain{parent},
      _items{*this, memory_datatype}

{
}


SpaceBoxDomain::SpaceBoxDomain(
    constant_collection::SpaceDomain&& space_domain)

    : SpaceBoxDomain{
        std::forward<constant_collection::SpaceDomain>(space_domain),
        hdf5::memory_datatype(file_datatype(space_domain))}

{
}


SpaceBoxDomain::SpaceBoxDomain(
    constant_collection::SpaceDomain&& space_domain,
    hdf5::Datatype const& memory_datatype)

    : constant_collection::SpaceDomain{
        std::forward<constant_collection::SpaceDomain>(space_domain)},
      _items{*this, memory_datatype}

{
}


SpaceBoxDomain::SpaceBoxes const& SpaceBoxDomain::items() const
{
    return _items;
}


SpaceBoxDomain::SpaceBoxes& SpaceBoxDomain::items()
{
    return _items;
}


SpaceBoxDomain::SpaceBoxes& SpaceBoxDomain::reserve(
    hsize_t const nr_items)
{
    _items.reserve(nr_items);

    return _items;
}


SpaceBoxDomain create_space_box_domain(
    PropertySet& property_set,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    auto& domain = property_set.domain();

    SpaceConfiguration const configuration{
        Occurrence::continuous,
        CollectionVariability::constant,
        Mobility::stationary,
        SpaceDomainItemType::box
    };

    auto space =
        constant_collection::create_space_domain(domain, configuration);

    stationary::create_box(space, file_datatype, memory_datatype, rank);

    return SpaceBoxDomain(std::move(space), memory_datatype);
}

}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
