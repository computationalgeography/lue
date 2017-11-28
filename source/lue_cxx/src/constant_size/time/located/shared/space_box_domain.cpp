#include "lue/constant_size/time/located/shared/space_box_domain.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

hdf5::Datatype SpaceBoxDomain::file_datatype(
    hdf5::Group const& parent)
{
    return hdf5::Dataset(parent, coordinates_tag).datatype();
}


SpaceBoxDomain::SpaceBoxDomain(
    SpaceDomain&& space_domain)

    : SpaceBoxDomain{
        std::forward<SpaceDomain>(space_domain),
        hdf5::memory_datatype(file_datatype(space_domain))}

{
}


SpaceBoxDomain::SpaceBoxDomain(
    SpaceDomain&& space_domain,
    hdf5::Datatype const& memory_datatype)

    : SpaceDomain{std::forward<SpaceDomain>(space_domain)},
      _items{*this, memory_datatype}

{
}


SpaceBox const& SpaceBoxDomain::items() const
{
    return _items;
}


SpaceBox& SpaceBoxDomain::items()
{
    return _items;
}


SpaceBox& SpaceBoxDomain::reserve(
    hsize_t const nr_items)
{
    // TODO This assumes this information doesn't change over time...
    _items.reserve(1, nr_items);

    return _items;
}


SpaceBoxDomain create_space_box_domain(
    PropertySet& property_set,
    hdf5::Datatype const file_datatype,
    hdf5::Datatype const memory_datatype,
    size_t const rank)
{
    auto& domain = property_set.domain();

    auto space = shared::create_space_domain(domain,
        SpaceDomain::Configuration(
            SpaceDomain::Configuration::DomainType::located,
            SpaceDomain::Configuration::ItemType::box)
    );

    create_space_box(space, file_datatype, memory_datatype, rank);

    return SpaceBoxDomain(std::move(space), memory_datatype);
}

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
