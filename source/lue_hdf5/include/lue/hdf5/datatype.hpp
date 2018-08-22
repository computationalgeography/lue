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

    explicit       Datatype            (hid_t id);

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


static Datatype const native_uint8{H5T_NATIVE_UINT8};
static Datatype const native_uint16{H5T_NATIVE_UINT16};
static Datatype const native_uint32{H5T_NATIVE_UINT32};
static Datatype const native_uint64{H5T_NATIVE_UINT64};
static Datatype const native_int8{H5T_NATIVE_INT8};
static Datatype const native_int16{H5T_NATIVE_INT16};
static Datatype const native_int32{H5T_NATIVE_INT32};
static Datatype const native_int64{H5T_NATIVE_INT64};
static Datatype const native_float32{H5T_NATIVE_FLOAT};
static Datatype const native_float64{H5T_NATIVE_DOUBLE};

static Datatype const std_uint8_le{H5T_STD_U8LE};
static Datatype const std_uint16_le{H5T_STD_U16LE};
static Datatype const std_uint32_le{H5T_STD_U32LE};
static Datatype const std_uint64_le{H5T_STD_U64LE};
static Datatype const std_int8_le{H5T_STD_I8LE};
static Datatype const std_int16_le{H5T_STD_I16LE};
static Datatype const std_int32_le{H5T_STD_I32LE};
static Datatype const std_int64_le{H5T_STD_I64LE};
static Datatype const ieee_float32_le{H5T_IEEE_F32LE};
static Datatype const ieee_float64_le{H5T_IEEE_F64LE};

} // namespace hdf5
} // namespace lue
