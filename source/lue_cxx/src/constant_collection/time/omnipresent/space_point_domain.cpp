#include "lue/constant_collection/time/omnipresent/space_point_domain.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {

hdf5::Datatype SpacePointDomain::file_datatype(
    hdf5::Group const& parent)
{
    return hdf5::Dataset(parent, coordinates_tag).datatype();
}


SpacePointDomain::SpacePointDomain(
    SpaceDomain&& space_domain)

    : SpacePointDomain{
        std::forward<SpaceDomain>(space_domain),
        hdf5::memory_datatype(file_datatype(*this))}

{
}


SpacePointDomain::SpacePointDomain(
    SpaceDomain&& space_domain,
    hdf5::Datatype const& memory_datatype)

    : SpaceDomain{std::forward<SpaceDomain>(space_domain)},
      _items{*this, coordinates_tag, memory_datatype}

{
}


SpacePointDomain::SpacePoints const& SpacePointDomain::items() const
{
    return _items;
}


SpacePointDomain::SpacePoints& SpacePointDomain::items()
{
    return _items;
}


SpacePointDomain::SpacePoints& SpacePointDomain::reserve(
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

    // A point is defined by the coordinates along each dimension
    hdf5::Shape value_shape = { rank };

    constant_shape::same_shape::create_constant(
        space, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return SpacePointDomain(std::move(space), memory_datatype);
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
