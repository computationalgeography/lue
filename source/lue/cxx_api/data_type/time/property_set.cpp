#include "lue/cxx_api/data_type/time/property_set.h"


namespace lue {
namespace api {

PropertySet::PropertySet(
    lue::PropertySet& group)

    : _group{std::ref(group)}

{
}


hdf5::Identifier const& PropertySet::id() const
{
    return group().id();
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
    std::string const& name)
{
    return group().add_property(name);
}

} // namespace api
} // namespace lue
