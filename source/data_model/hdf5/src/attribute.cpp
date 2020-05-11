#include "lue/hdf5/attribute.hpp"


namespace lue {
namespace hdf5 {

/*!
    @brief      Construct instance based on @a id
    @param      id Identifier of attribute
*/
Attribute::Attribute(
    Identifier&& id)

    : _id{std::forward<Identifier>(id)},
      _datatype{::H5Aget_type(_id)},
      _dataspace{::H5Aget_space(_id)}

{
}


/*!
    @brief      Open attribute named @a name
    @param      location Location of attribute
    @param      name Name of attribute
    @exception  std::runtime_error In case attribute cannot be opened
*/
Attribute::Attribute(
    Identifier const& location,
    std::string const& name)

    : _id(::H5Aopen(location, name.c_str(), H5P_DEFAULT), ::H5Aclose),
      _datatype{::H5Aget_type(_id)},
      _dataspace{::H5Aget_space(_id)}

{
}


/*!
    @brief      Return the id of the attribute
*/
Identifier const& Attribute::id() const
{
    return _id;
}


/*!
    @brief      Return the datatype of the attribute
*/
Datatype const& Attribute::datatype() const
{
    return _datatype;
}


/*!
    @brief      Return the dataspace of the attribute
*/
Dataspace const& Attribute::dataspace() const
{
    return _dataspace;
}


/*!
    @brief      Create attribute
    @param      location Location
    @param      name Name
    @param      datatype Datatype
    @param      dataspace Dataspace
    @exception  std::runtime_error In case attribute cannot be created
*/
Attribute create_attribute(
    Identifier const& location,
    std::string const& name,
    Datatype const& datatype,
    Dataspace const& dataspace)
{
    Identifier id{
        ::H5Acreate(location, name.c_str(), datatype.id(),
        dataspace.id(), H5P_DEFAULT, H5P_DEFAULT), ::H5Aclose};

    if(!id.is_valid()) {
        throw std::runtime_error("Cannot create attribute");
    }

    return Attribute(std::move(id));
}

} // namespace hdf5
} // namespace lue
