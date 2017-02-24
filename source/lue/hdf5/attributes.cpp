#include "lue/hdf5/attributes.h"
#include "lue/hdf5/attribute.h"
#include <cassert>


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


template<>
void Attributes::write<std::string>(
    std::string const& name,
    std::string const& value)
{
    assert(_id.get().is_valid());
    assert(!exists(name));

    auto attribute = create_attribute(_id, name, value);
}


template<
    typename T>
void Attributes::write(
    std::string const& name,
    T const& value)
{
    assert(_id.get().is_valid());
    assert(!exists(name));

    auto const datatype = Datatype(value);
    auto const dataspace = Dataspace(::H5S_SCALAR);
    auto attribute = create_attribute(_id, name, datatype, dataspace);

    attribute.write(value);
}


template<
    typename T>
T Attributes::read(
    std::string const& name) const
{
    assert(_id.get().is_valid());
    assert(exists(name));

    auto const attribute = Attribute(_id, name);
    auto value = attribute.read<T>();

    return value;
}


template
std::string Attributes::read(std::string const&) const;


bool Attributes::exists(
    std::string const& name) const
{
    return ::H5Aexists(_id.get(), name.c_str()) > 0;
}


Datatype Attributes::datatype(
    std::string const& name) const
{
    assert(_id.get().is_valid());
    assert(exists(name));

    auto const attribute = Attribute(_id, name);
    auto const datatype = attribute.datatype();

    return datatype;
}

} // namespace hdf5
} // namespace lue
