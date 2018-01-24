#include "lue/constant_collection/time/located/property.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace located {

Property::Property(
    constant_collection::Property&& property)

    : constant_collection::Property(std::forward<constant_collection::Property>(property))

{
}


void Property::discretize_time(
    lue::Property const& property)
{
    create_soft_link(property.id(), time_discretization_tag);
}


bool Property::time_is_discretized () const
{
    return contains_soft_link(time_discretization_tag);
}


lue::Property Property::time_discretization () const
{
    return lue::Property(*this, time_discretization_tag);
}


Property create_property(
    hdf5::Group& parent,
    std::string const& name,
    Configuration const& configuration)
{
    auto property = constant_collection::create_property(parent, name, configuration);

    return Property{std::move(property)};
}

}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
