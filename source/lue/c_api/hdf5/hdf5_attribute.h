#pragma once
#include <hdf5.h>


/*!
    @file
    @ingroup    lue_c_api_hdf5_group
    @sa         - https://www.hdfgroup.org/HDF5/doc/RM/RM_H5A.html
                - https://www.hdfgroup.org/HDF5/doc/UG/13_Attributes.html
                - https://www.hdfgroup.org/ftp/HDF5/examples/misc-examples/attrs.c
                - https://www.hdfgroup.org/ftp/HDF5/current/src/unpacked/examples/h5_attribute.c
*/


#ifdef __cplusplus
extern "C" {
#endif


hid_t              hdf5_create_attribute(
                                        hid_t const location_id,
                                        char const* name,
                                        hid_t const type_id,
                                        hid_t const space_id);

hid_t              hdf5_open_attribute (hid_t const location_id,
                                        char const* name);

herr_t             hdf5_read_attribute (hid_t const location_id,
                                        hid_t const type_id,
                                        void* buffer);

herr_t             hdf5_write_attribute(hid_t const location_id,
                                        hid_t const type_id,
                                        void const* buffer);

herr_t             hdf5_close_attribute(hid_t const location_id);

htri_t             hdf5_attribute_exists(
                                        hid_t const location_id,
                                        char const* name);

hid_t              hdf5_attribute_dataspace(
                                        hid_t const location_id);

hid_t              hdf5_attribute_datatype(
                                        hid_t const location_id);

hid_t              hdf5_attribute_property_list(
                                        hid_t const attribute_id);


herr_t             hdf5_write_string_attribute(
                                        hid_t const location_id,
                                        char const* name,
                                        char const* value);

herr_t             hdf5_read_string_attribute(
                                        hid_t const location_id,
                                        char const* name,
                                        char** value);


#define declare_attribute_functions(                              \
    type_name,                                                    \
    type)                                                         \
                                                                  \
herr_t             hdf5_create_##type_name##_attribute(           \
                                        hid_t const location_id,  \
                                        char const* name);        \
                                                                  \
herr_t             hdf5_write_##type_name##_attribute(            \
                                        hid_t const location_id,  \
                                        char const* name,         \
                                        type const value);        \
                                                                  \
herr_t             hdf5_read_##type_name##_attribute(             \
                                        hid_t const location_id,  \
                                        char const* name,         \
                                        type* value);


declare_attribute_functions(int8, int8_t)
declare_attribute_functions(int16, int16_t)
declare_attribute_functions(int32, int32_t)
declare_attribute_functions(int64, int64_t)
declare_attribute_functions(uint8, uint8_t)
declare_attribute_functions(uint16, uint16_t)
declare_attribute_functions(uint32, uint32_t)
declare_attribute_functions(uint64, uint64_t)
declare_attribute_functions(float, float)
declare_attribute_functions(double, double)
declare_attribute_functions(hsize, hsize_t)

#undef declare_attribute_functions


#ifdef __cplusplus
}  // extern "C"
#endif
