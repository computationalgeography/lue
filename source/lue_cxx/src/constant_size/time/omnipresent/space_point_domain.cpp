#include "lue/constant_size/time/omnipresent/space_point_domain.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

hdf5::Datatype SpacePointDomain::file_datatype(
    hdf5::Identifier const& id)
{
    return hdf5::Dataset(id, coordinates_tag).datatype();
}


SpacePointDomain::SpacePointDomain(
    SpaceDomain const& space_domain)

    : SpaceDomain(space_domain),
      _items(id(), hdf5::memory_datatype(file_datatype(space_domain.id())))

{
}


SpacePointDomain::SpacePointDomain(
    SpaceDomain const& space_domain,
    hdf5::Datatype const& memory_datatype)

    : SpaceDomain(space_domain),
      _items(id(), memory_datatype)

{
}


SpacePointDomain::SpacePointDomain(
    SpaceDomain&& space_domain,
    hdf5::Datatype const& memory_datatype)

    : SpaceDomain(std::forward<SpaceDomain>(space_domain)),
      _items(id(), memory_datatype)

{
}


SpacePoint const& SpacePointDomain::items() const
{
    return _items;
}


SpacePoint& SpacePointDomain::items()
{
    return _items;
}


SpacePoint& SpacePointDomain::reserve(
    hsize_t const nr_items)
{
    _items.reserve(nr_items);

    return _items;
}


SpacePointDomain create_space_point_domain(
    PropertySet& property_set,
    hdf5::Datatype const file_datatype,
    hdf5::Datatype const memory_datatype,
    size_t const rank)
{
    auto& domain = property_set.domain();

    auto space = omnipresent::create_space_domain(domain,
        SpaceDomain::Configuration(
            SpaceDomain::Configuration::DomainType::located,
            SpaceDomain::Configuration::ItemType::point)
    );

    create_space_point(space, file_datatype, memory_datatype, rank);

    return SpacePointDomain(space, memory_datatype);
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
