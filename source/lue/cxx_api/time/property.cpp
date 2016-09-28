#include "lue/cxx_api/time/property.h"


namespace lue {
namespace time {

Property::Property(
    lue::Property& group)

    : _group{std::ref(group)}

{
}


hdf5::Identifier const& Property::id() const
{
    return group().id();
}


std::string Property::name() const
{
    return group().name();
}


void Property::link_space_discretization(
    lue::Property const& discretization)
{
    group().link_space_discretization(discretization);
}


lue::Property const& Property::group() const
{
    return _group.get();
}


lue::Property& Property::group()
{
    return _group.get();
}

} // namespace time
} // namespace lue
