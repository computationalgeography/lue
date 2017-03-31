#include "lue/hdf5/attributes.h"


namespace lue {
namespace hdf5 {

/*!
    @brief      Construct collection based on @a id
    @param      id Id of object containing attributes
*/
Attributes::Attributes(
    Identifier const& id)

    : _id{id}

{
}


/*!
    @brief      Return whether an attribute named @a name is present
                in the layered object
*/
bool Attributes::exists(
    std::string const& name) const
{
    return ::H5Aexists(_id, name.c_str()) > 0;
}


/*!
    @brief      Return the attribute named @a name
*/
Attribute Attributes::attribute(
    std::string const& name) const
{
    assert(_id.is_valid());
    assert(exists(name));

    return Attribute(_id, name);
}


/*!
    @brief      Return the datatype of the attribute named @a name
*/
Datatype Attributes::datatype(
    std::string const& name) const
{
    return attribute(name).datatype();
}

} // namespace hdf5
} // namespace lue
