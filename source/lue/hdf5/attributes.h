#pragma once
#include "lue/hdf5/attribute.h"
#include "lue/hdf5/datatype.h"
#include "lue/hdf5/datatype_traits.h"
#include <cassert>


namespace lue {
namespace hdf5 {

class Attributes
{

public:

                   Attributes          (Identifier const& id);

                   Attributes          (Attributes const& other)=default;

                   Attributes          (Attributes&& other)=default;

                   ~Attributes         ()=default;

    Attributes&    operator=           (Attributes const& other)=default;

    Attributes&    operator=           (Attributes&& other)=default;

    template<
        typename T>
    void           write               (std::string const& name,
                                        T const& value);

    template<
        typename T>
    T              read                (std::string const& name) const;

    bool           exists              (std::string const& name) const;

    Attribute      attribute           (std::string const& name) const;

    Datatype       datatype            (std::string const& name) const;

private:

    std::reference_wrapper<Identifier const> _id;

};


template<
    typename T>
inline T Attributes::read(
    std::string const& name) const
{
    assert(_id.get().is_valid());
    assert(exists(name));

    auto const attribute = Attribute(_id, name);
    auto value = attribute.read<T>();

    return value;
}


// template<>
// inline void Attributes::write<std::string>(
//     std::string const& name,
//     std::string const& value)
// {
//     assert(_id.get().is_valid());
//     assert(!exists(name));
// 
//     auto attribute = create_attribute(_id, name, value);
// }
// 
// 
// template<>
// inline void Attributes::write<std::vector<unsigned char>>(
//     std::string const& name,
//     std::vector<unsigned char> const& value)
// {
//     assert(_id.get().is_valid());
//     assert(!exists(name));
// 
//     auto attribute = create_attribute(_id, name, value);
// }


template<
    typename T>
inline void Attributes::write(
    std::string const& name,
    T const& value)
{
    assert(_id.get().is_valid());

    if(!exists(name)) {
        create_attribute(_id, name, value);
    }
    else {
        this->attribute(name).write<T>(value);
    }
}

} // namespace hdf5
} // namespace lue
