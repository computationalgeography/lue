#include "lue/cxx_api/constant_size/time/property_set.h"


namespace lue {
namespace time {

PropertySet::PropertySet(
    lue::PropertySet& group)

    : _group{std::ref(group)}

{
}


hdf5::Identifier const& PropertySet::id() const
{
    return group().id();
}


PropertySetConfiguration const& PropertySet::configuration() const
{
    return group().configuration();
}


hdf5::Attributes const& PropertySet::attributes() const
{
    return group().attributes();
}


Domain& PropertySet::domain() const
{
    return group().domain();
}


Properties& PropertySet::properties() const
{
    return group().properties();
}


lue::PropertySet const& PropertySet::group() const
{
    return _group.get();
}


lue::PropertySet& PropertySet::group()
{
    return _group.get();
}


lue::Property& PropertySet::add_property(
    std::string const& name,
    ValueConfiguration const& value_configuration)
{
    return group().add_property(name, value_configuration);
}

} // namespace time
} // namespace lue
