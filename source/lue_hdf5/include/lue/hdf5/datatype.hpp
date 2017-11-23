#pragma once
#include "lue/hdf5/identifier.hpp"
#include <vector>


namespace lue {
namespace hdf5 {

/*!
    @brief      Class representing an HDF5 datatype
    @sa         [HDF5 predefined datatypes](https://support.hdfgroup.org/HDF5/doc/RM/PredefDTypes.html)
*/
class Datatype
{

public:

    // template<
    //     typename T>
    // explicit       Datatype            (T const&);

                   Datatype            ()=default;

                   Datatype            (hid_t id);

    explicit       Datatype            (Identifier&& id);

                   Datatype            (Datatype const&)=default;

                   Datatype            (Datatype&&)=default;

                   ~Datatype           ()=default;

    Datatype&      operator=           (Datatype const&)=default;

    Datatype&      operator=           (Datatype&&)=default;

    bool           operator==          (Datatype const& other) const;

    bool           operator!=          (Datatype const& other) const;

    Identifier const& id               () const;

    void           set_size            (std::size_t nr_bytes);

    std::size_t    size                () const;

    ::H5T_cset_t   encoding            () const;

    bool           is_standard         () const;

    bool           is_native           () const;

    void           insert              (std::string const& name,
                                        std::size_t offset,
                                        Datatype const& datatype);

private:

    //! Identifier of the datatype
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


std::vector<unsigned char> encode_datatype(
                                        Datatype const& datatype);

Datatype           decode_datatype     (std::vector<unsigned char> const&
                                            buffer);


std::string        native_datatype_as_string(
                                        Datatype const& datatype);

std::string        standard_datatype_as_string(
                                        Datatype const& datatype);

Datatype           create_datatype     (std::size_t nr_bytes);

Datatype           create_datatype     (hid_t type_id,
                                        std::size_t nr_bytes);

Datatype           create_string_datatype();

Datatype           create_compound_datatype(
                                        std::size_t nr_bytes);

Datatype           memory_datatype     (Datatype const& file_datatype);

Datatype           file_datatype       (Datatype const& memory_datatype);

} // namespace hdf5
} // namespace lue
