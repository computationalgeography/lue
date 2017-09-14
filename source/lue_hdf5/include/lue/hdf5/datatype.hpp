#pragma once
#include "lue/hdf5/identifier.hpp"
// #include "lue/hdf5/type_traits.hpp"
#include <vector>


namespace lue {
namespace hdf5 {

class Datatype
{

public:

    // template<
    //     typename T>
    // explicit       Datatype            (T const&);

                   Datatype            (hid_t const id);

                   Datatype            (Identifier&& id);

                   Datatype            (Datatype const& other)=default;

                   Datatype            (Datatype&& other)=default;

                   ~Datatype           ()=default;

    Datatype&      operator=           (Datatype const& other)=default;

    Datatype&      operator=           (Datatype&& other)=default;

    Identifier const& id               () const;

    void           set_size            (size_t const nr_bytes);

    size_t         size                () const;

    ::H5T_cset_t   encoding            () const;

    bool           is_standard         () const;

    bool           is_native           () const;

private:

    Identifier     _id;

};


// template<
//     typename T>
// inline Datatype::Datatype(
//     T const&)
// 
//     : _id(Traits<T>::type_id, [](hid_t){})
// 
// {
// }


bool               operator==          (Datatype const& lhs,
                                        Datatype const& rhs);

bool               operator!=          (Datatype const& lhs,
                                        Datatype const& rhs);


std::vector<unsigned char> encode_datatype(
                                        Datatype const& datatype);

Datatype           decode_datatype     (std::vector<unsigned char> const&
                                            buffer);


std::string        native_datatype_as_string(
                                        Datatype const& datatype);

std::string        standard_datatype_as_string(
                                        Datatype const& datatype);

Datatype           create_datatype     (size_t const nr_bytes);

Datatype           create_datatype     (hid_t const type_id,
                                        size_t const nr_bytes);

Datatype           memory_datatype     (Datatype const& file_datatype);

Datatype           file_datatype       (Datatype const& memory_datatype);

} // namespace hdf5
} // namespace lue
