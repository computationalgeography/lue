#pragma once
#include "lue/hdf5/dataspace.hpp"
#include "lue/hdf5/datatype.hpp"
#include "lue/hdf5/datatype_traits.hpp"
#include <cassert>
#include <stdexcept>
#include <type_traits>


namespace lue {
namespace hdf5 {
namespace detail {

template<
    typename T>
inline std::enable_if_t<!std::is_base_of<std::vector<hsize_t>, T>::value>
        read_attribute(
    Identifier const& id,
    Datatype const& datatype,
    Dataspace const& /* dataspace */,
    T& value)
{
    auto status = ::H5Aread(id, datatype.id(), &value);

    if(status < 0) {
        throw std::runtime_error("Cannot read attribute");
    }
}


template<>
inline void read_attribute<std::string>(
    Identifier const& id,
    Datatype const& datatype,
    Dataspace const& /* dataspace */,
    std::string& value)
{
    static_assert(std::is_same<std::string::value_type, char>::value,
        "expect std::string::value_type to be char");

    assert(datatype.encoding() == ::H5T_CSET_UTF8);

    auto const nr_bytes = datatype.size();

    value.resize(nr_bytes, 'x');

    auto status = ::H5Aread(id, datatype.id(), value.data());
    assert(value.size() == nr_bytes);

    if(status < 0) {
        throw std::runtime_error("Cannot read attribute");
    }
}


template<
    typename T>
inline void read_attribute(
    Identifier const& id,
    Datatype const& datatype,
    Dataspace const& dataspace,
    std::vector<T>& value)
{
    auto const nr_elements = dataspace.nr_elements();

    value.resize(nr_elements);

    auto status = ::H5Aread(id, datatype.id(), value.data());
    assert(value.size() == static_cast<std::size_t>(nr_elements));

    if(status < 0) {
        throw std::runtime_error("Cannot read attribute");
    }
}


template<
    typename T>
inline std::enable_if_t<!std::is_base_of<std::vector<hsize_t>, T>::value>
        write_attribute(
    Identifier const& id,
    Datatype const& datatype,
    T const& value)
{
    auto status = ::H5Awrite(id, datatype.id(), &value);

    if(status < 0) {
        throw std::runtime_error("Cannot write attribute");
    }
}


template<>
inline void write_attribute<std::string>(
    Identifier const& id,
    Datatype const& datatype,
    std::string const& value)
{
    auto status = ::H5Awrite(id, datatype.id(), value.c_str());

    if(status < 0) {
        throw std::runtime_error("Cannot write attribute");
    }
}


template<
    typename T>
inline void write_attribute(
    Identifier const& id,
    Datatype const& datatype,
    std::vector<T> const& value)
{
    auto status = ::H5Awrite(id, datatype.id(), value.data());

    if(status < 0) {
        throw std::runtime_error("Cannot write attribute");
    }
}

}  // namespace detail



/*!
    @brief      This class represents an attribute
*/
class Attribute
{

public:

    explicit       Attribute           (Identifier&& id);

                   Attribute           (Identifier const& location,
                                        std::string const& name);

                   Attribute           (Attribute const&)=default;

                   Attribute           (Attribute&&)=default;

                   ~Attribute          ()=default;

    Attribute&     operator=           (Attribute const&)=default;

    Attribute&     operator=           (Attribute&&)=default;

    Identifier const& id               () const;

    Datatype const& datatype           () const;

    Dataspace const& dataspace         () const;

    template<
        typename T>
    void           write               (T const& value);

    template<
        typename T>
    void           write               (Datatype const& memory_datatype,
                                        T const& value);

    template<
        typename T>
    T              read                () const;

    template<
        typename T>
    void           read                (Datatype const& memory_datatype,
                                        T& value);

private:

    //! Identifier of attribute
    Identifier     _id;

    //! Datatype of attribute
    Datatype       _datatype;

    //! Dataspace of attribute
    Dataspace      _dataspace;

};


/*!
    @brief      Read attribute value
    @tparam     T Type of value to read
    @exception  std::runtime_error In case the attribute cannot be read
    @warning    It is assumed that @a T corresponds with the attribute's
                datatype
*/
template<
    typename T>
inline T Attribute::read() const
{
    T value;
    detail::read_attribute(id(), datatype(), dataspace(), value);

    return value;
}


template<
    typename T>
inline void Attribute::read(
    Datatype const& memory_datatype,
    T& value)
{
    detail::read_attribute(id(), memory_datatype, dataspace(), value);
}


/*!
    @brief      Write attribute value
    @tparam     T Type of value to write
    @exception  std::runtime_error In case the attribute cannot be written
    @warning    It is assumed that @a T corresponds with the attribute's
                datatype
*/
template<
    typename T>
inline void Attribute::write(
    T const& value)
{
    detail::write_attribute(id(), datatype(), value);
}


template<
    typename T>
inline void Attribute::write(
    Datatype const& memory_datatype,
    T const& value)
{
    detail::write_attribute(id(), memory_datatype, value);
}


Attribute          create_attribute    (Identifier const& location,
                                        std::string const& name,
                                        Datatype const& datatype,
                                        Dataspace const& dataspace);


/*!
    @brief      Create attribute and initialize its value
    @param      location Location
    @param      name Name
    @param      value Initial value
    @sa         create_attribute(Identifier const&, std::string const&,
                Datatype const&, Dataspace const&)

    This overload is enabled for all types that don't enherit from
    std::vector<hsize_t>. This is done to let the compiler pick the
    overload for std::vector<T> for Shape, for example.
*/
template<
    typename T>
inline std::enable_if_t<
    !std::is_base_of<std::vector<hsize_t>, T>::value, Attribute>
        create_attribute(
    Identifier const& location,
    std::string const& name,
    T const& value)
{
    auto datatype = Datatype{NativeDatatypeTraits<T>::type_id()};
    auto dataspace = Dataspace{::H5S_SCALAR};
    auto attribute = create_attribute(location, name, datatype, dataspace);

    attribute.write(value);

    return attribute;
}


template<>
inline Attribute create_attribute(
    Identifier const& location,
    std::string const& name,
    std::string const& value)
{
    auto datatype = create_datatype(value.size());
    auto dataspace = Dataspace{::H5S_SCALAR};
    auto attribute = create_attribute(location, name, datatype, dataspace);

    attribute.write(value);

    return attribute;
}


template<
    typename T>
inline Attribute create_attribute(
    Identifier const& location,
    std::string const& name,
    std::vector<T> const& value)
{
    auto datatype = Datatype{NativeDatatypeTraits<T>::type_id()};
    auto dataspace = create_dataspace(Shape{value.size()});
    auto attribute = create_attribute(location, name, datatype, dataspace);

    attribute.write(value);

    return attribute;
}

} // namespace hdf5
} // namespace lue
