#include "lue/c_api/hdf5/hdf5_attribute.h"
#include "lue/c_api/hdf5/hdf5_dataspace.h"
#include "lue/c_api/hdf5/hdf5_datatype.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>


/*!
    @ingroup    lue_c_api_hdf5_group
*/
hid_t hdf5_create_attribute(
    hid_t const location_id,
    char const* name,
    hid_t const type_id,
    hid_t const space_id)
{
    return H5Acreate(location_id, name, type_id, space_id, H5P_DEFAULT,
        H5P_DEFAULT);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
hid_t hdf5_open_attribute(
    hid_t const location_id,
    char const* name)
{
    return H5Aopen(location_id, name, H5P_DEFAULT);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
herr_t hdf5_read_attribute(
    hid_t const location_id,
    hid_t const type_id,
    void* buffer)
{
    return H5Aread(location_id, type_id, buffer);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
herr_t hdf5_write_attribute(
    hid_t const location_id,
    hid_t const type_id,
    void const* buffer)
{
    return H5Awrite(location_id, type_id, buffer);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
herr_t hdf5_close_attribute(
    hid_t const location_id)
{
    return H5Aclose(location_id);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
htri_t hdf5_attribute_exists(
    hid_t const location_id,
    char const* name)
{
    return H5Aexists(location_id, name);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
hid_t hdf5_attribute_dataspace(
    hid_t const location_id)
{
    return H5Aget_space(location_id);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
hid_t hdf5_attribute_datatype(
    hid_t const location_id)
{
    return H5Aget_type(location_id);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
hid_t hdf5_attribute_property_list(
    hid_t const attribute_id)
{
    return H5Aget_create_plist(attribute_id);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
herr_t hdf5_write_string_attribute(
    hid_t const location_id,
    char const* name,
    char const* value)
{
    hid_t datatype_id = hdf5_create_string_datatype(strlen(value));

    if(datatype_id < 0) {
        return datatype_id;
    }


    hid_t dataspace_id = H5Screate(H5S_SCALAR);

    if(dataspace_id < 0) {
        hdf5_close_datatype(datatype_id);
        return dataspace_id;
    }


    hid_t attribute_id = hdf5_create_attribute(location_id, name,
        datatype_id, dataspace_id);

    if(attribute_id < 0) {
        hdf5_close_dataspace(dataspace_id);
        hdf5_close_datatype(datatype_id);
        return attribute_id;
    }


    herr_t status = hdf5_write_attribute(attribute_id, datatype_id, value);

    if(status < 0) {
        hdf5_close_attribute(attribute_id);
        hdf5_close_dataspace(dataspace_id);
        hdf5_close_datatype(datatype_id);
        return status;
    }


    // Wrap-up.
    status = hdf5_close_attribute(attribute_id);

    if(status < 0) {
        hdf5_close_dataspace(dataspace_id);
        hdf5_close_datatype(datatype_id);
        return status;
    }


    status = hdf5_close_dataspace(dataspace_id);

    if(status < 0) {
        hdf5_close_datatype(datatype_id);
        return status;
    }


    return hdf5_close_datatype(datatype_id);
}


herr_t hdf5_read_string_attribute(
    hid_t const location_id,
    char const* name,
    char** value)
{
    hid_t attribute_id = hdf5_open_attribute(location_id, name);

    if(attribute_id < 0) {
        return attribute_id;
    }


    hid_t datatype_id = hdf5_attribute_datatype(attribute_id);

    if(datatype_id < 0) {
        hdf5_close_attribute(attribute_id);
        return datatype_id;
    }

    assert(hdf5_datatype_encoding(datatype_id) == H5T_CSET_UTF8);


    // Reserve space for the character bytes and a trailing '\0'.
    size_t nr_bytes = hdf5_nr_bytes(datatype_id);
    *value = (char*)malloc((nr_bytes + 1) * sizeof(char));
    (*value)[nr_bytes] = '\0';

    herr_t status = hdf5_read_attribute(attribute_id, datatype_id, *value);

    if(status < 0) {
        hdf5_close_datatype(datatype_id);
        hdf5_close_attribute(attribute_id);
        return status;
    }

    assert((*value)[nr_bytes] == '\0');
    assert(strlen(*value) == nr_bytes);


    // Wrap-up.
    status = hdf5_close_datatype(datatype_id);

    if(status < 0) {
        hdf5_close_attribute(attribute_id);
        return status;
    }


    return hdf5_close_attribute(attribute_id);
}


#define define_attribute_functions(                                           \
    type_name,                                                                \
    type,                                                                     \
    hdf5_type)                                                                \
                                                                              \
herr_t hdf5_create_##type_name##_attribute(                                   \
    hid_t const location_id,                                                  \
    char const* name)                                                         \
{                                                                             \
    hid_t datatype_id = hdf5_type;                                            \
                                                                              \
                                                                              \
    hid_t dataspace_id = H5Screate(H5S_SCALAR);                               \
                                                                              \
    if(dataspace_id < 0) {                                                    \
        return dataspace_id;                                                  \
    }                                                                         \
                                                                              \
                                                                              \
    hid_t attribute_id = hdf5_create_attribute(location_id, name,             \
        datatype_id, dataspace_id);                                           \
                                                                              \
    if(attribute_id < 0) {                                                    \
        hdf5_close_dataspace(dataspace_id);                                   \
        return attribute_id;                                                  \
    }                                                                         \
                                                                              \
                                                                              \
    herr_t status = hdf5_close_attribute(attribute_id);                       \
                                                                              \
    if(status < 0) {                                                          \
        hdf5_close_dataspace(dataspace_id);                                   \
        return status;                                                        \
    }                                                                         \
                                                                              \
                                                                              \
    return hdf5_close_dataspace(dataspace_id);                                \
}                                                                             \
                                                                              \
                                                                              \
herr_t hdf5_write_##type_name##_attribute(                                    \
    hid_t const location_id,                                                  \
    char const* name,                                                         \
    type const value)                                                         \
{                                                                             \
    hid_t attribute_id = hdf5_open_attribute(location_id, name);              \
                                                                              \
    if(attribute_id < 0) {                                                    \
        return attribute_id;                                                  \
    }                                                                         \
                                                                              \
                                                                              \
    hid_t datatype_id = hdf5_attribute_datatype(attribute_id);                \
                                                                              \
    if(datatype_id < 0) {                                                     \
        hdf5_close_attribute(attribute_id);                                   \
        return datatype_id;                                                   \
    }                                                                         \
                                                                              \
                                                                              \
    herr_t status = hdf5_write_attribute(attribute_id, datatype_id, &value);  \
                                                                              \
    if(status < 0) {                                                          \
        hdf5_close_datatype(datatype_id);                                     \
        hdf5_close_attribute(attribute_id);                                   \
        return status;                                                        \
    }                                                                         \
                                                                              \
                                                                              \
    status = hdf5_close_datatype(datatype_id);                                \
                                                                              \
    if(status < 0) {                                                          \
        hdf5_close_attribute(attribute_id);                                   \
        return status;                                                        \
    }                                                                         \
                                                                              \
                                                                              \
    return hdf5_close_attribute(attribute_id);                                \
    }                                                                         \
                                                                              \
                                                                              \
herr_t hdf5_read_##type_name##_attribute(                                     \
    hid_t const location_id,                                                  \
    char const* name,                                                         \
    type* value)                                                              \
{                                                                             \
    hid_t attribute_id = hdf5_open_attribute(location_id, name);              \
                                                                              \
    if(attribute_id < 0) {                                                    \
        return attribute_id;                                                  \
    }                                                                         \
                                                                              \
                                                                              \
    hid_t datatype_id = hdf5_attribute_datatype(attribute_id);                \
                                                                              \
    if(datatype_id < 0) {                                                     \
        hdf5_close_attribute(attribute_id);                                   \
        return datatype_id;                                                   \
    }                                                                         \
                                                                              \
                                                                              \
    herr_t status = hdf5_read_attribute(attribute_id, datatype_id, value);    \
                                                                              \
    if(status < 0) {                                                          \
        hdf5_close_datatype(datatype_id);                                     \
        hdf5_close_attribute(attribute_id);                                   \
        return status;                                                        \
    }                                                                         \
                                                                              \
                                                                              \
    status = hdf5_close_datatype(datatype_id);                                \
                                                                              \
    if(status < 0) {                                                          \
        hdf5_close_attribute(attribute_id);                                   \
        return status;                                                        \
    }                                                                         \
                                                                              \
                                                                              \
    status = hdf5_close_attribute(attribute_id);                              \
    if(status < 0) {                                                          \
        return status;                                                        \
    }                                                                         \
                                                                              \
    return status;                                                            \
}


define_attribute_functions(int32, int32_t, H5T_STD_I32LE)
define_attribute_functions(uint32, uint32_t, H5T_STD_U32LE)
define_attribute_functions(uint64, uint64_t, H5T_STD_U64LE)
define_attribute_functions(hsize, hsize_t, H5T_STD_U64LE)

#undef define_attribute_functions
