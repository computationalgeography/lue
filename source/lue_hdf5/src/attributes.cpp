#include "lue/hdf5/attributes.hpp"


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
    @brief      Add a new attribute to the set
    @param      name Name of attribute
    @param      datatype In-file datatype of the value
    @param      dataspace Dataspace of the value
*/
Attribute Attributes::add(
    std::string const& name,
    Datatype const& datatype,
    Dataspace const& dataspace)
{
    return create_attribute(_id, name, datatype, dataspace);
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

    // if(!exists(name)) {
    //     throw std::runtime_error(
    //         name + ": no such attribute in " + _id.pathname());
    // }

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
