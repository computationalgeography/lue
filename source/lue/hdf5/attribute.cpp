#include "lue/hdf5/attribute.h"
#include <cassert>


namespace lue {
namespace hdf5 {

/*
    @brief      Create new attribute named @name
    @param      .
    @return     .
    @exception  std::runtime_error In case attribute cannot be created
Attribute::Attribute(
    Identifier const& location,
    std::string const& name,
    Datatype const& datatype,
    Dataspace const& dataspace)

    : _id(::H5Acreate(location, name.c_str(), datatype.id(), dataspace.id(),
          H5P_DEFAULT, H5P_DEFAULT), ::H5Aclose),
      _datatype{datatype}

{
    if(!_id.is_valid() < 0) {
        throw std::runtime_error("Cannot create attribute");
    }
}
*/


Attribute::Attribute(
    Identifier&& id)

    : _id{std::forward<Identifier>(id)},
      _datatype(::H5Aget_type(_id))

{
}


/*!
    @brief      Open attribute named @name
    @param      .
    @return     .
    @exception  std::runtime_error In case attribute cannot be opened
*/
Attribute::Attribute(
    Identifier const& location,
    std::string const& name)

    : _id(::H5Aopen(location, name.c_str(), H5P_DEFAULT), ::H5Aclose),
      _datatype(::H5Aget_type(_id))

{
}


Identifier const& Attribute::id() const
{
    return _id;
}


Datatype const& Attribute::datatype() const
{
    return _datatype;
}


template<>
void Attribute::write<std::string>(
    std::string const& value)
{
    auto status = H5Awrite(_id, _datatype.id(), value.c_str());

    if(status < 0) {
        throw std::runtime_error("Cannot write attribute");
    }
}


template<>
std::string Attribute::read<std::string>() const
{
    static_assert(std::is_same<std::string::value_type, char>::value,
        "expect std::string::value_type to be char");

    assert(_datatype.encoding() == ::H5T_CSET_UTF8);

    auto nr_bytes = _datatype.size();

    std::string result(nr_bytes, 'x');

    auto status = ::H5Aread(_id, _datatype.id(),
#if __cplusplus > 201402L
        result.data()
#else
        const_cast<std::string::value_type*>(result.data())
#endif
    );
    assert(result.size() == nr_bytes);

    if(status < 0) {
        throw std::runtime_error("Cannot read attribute");
    }

    return result;
}


Attribute create_attribute(
    Identifier const& location,
    std::string const& name,
    std::string const& value)
{
    auto attribute = create_attribute(location, name, value.size());
    attribute.write(value);

    return attribute;
}


Attribute create_attribute(
    Identifier const& location,
    std::string const& name,
    size_t const nr_bytes)
{
    return create_attribute(location, name, Datatype(nr_bytes),
        Dataspace(::H5S_SCALAR));
}


Attribute create_attribute(
    Identifier const& location,
    std::string const& name,
    Datatype const& datatype,
    Dataspace const& dataspace)
{
    Identifier id(::H5Acreate(location, name.c_str(), datatype.id(),
        dataspace.id(), H5P_DEFAULT, H5P_DEFAULT), ::H5Aclose);

    if(!id.is_valid() < 0) {
        throw std::runtime_error("Cannot create attribute");
    }

    return Attribute(std::move(id));
}

} // namespace hdf5
} // namespace lue
