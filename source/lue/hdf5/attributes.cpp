#include "lue/hdf5/attributes.h"


namespace lue {
namespace hdf5 {

/*!
    @brief      Construct collection based on @a id
    @param      id Id of object containing attributes
*/
Attributes::Attributes(
    Identifier const& id)

    : _id{std::cref(id)}

{
}


bool Attributes::exists(
    std::string const& name) const
{
    return ::H5Aexists(_id.get(), name.c_str()) > 0;
}


Attribute Attributes::attribute(
    std::string const& name) const
{
    assert(_id.get().is_valid());
    assert(exists(name));

    return Attribute(_id, name);
}


Datatype Attributes::datatype(
    std::string const& name) const
{
    return attribute(name).datatype();
}

} // namespace hdf5
} // namespace lue
