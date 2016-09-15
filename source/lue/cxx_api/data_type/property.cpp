#include "lue/cxx_api/data_type/property.h"


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
