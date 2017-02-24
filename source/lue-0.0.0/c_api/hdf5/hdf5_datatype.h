#pragma once
#include <hdf5.h>


/*!
    @file
    @ingroup    lue_c_api_hdf5_group
    @sa         https://www.hdfgroup.org/HDF5/doc/RM/RM_H5T.html
*/


#ifdef __cplusplus
extern "C" {
#endif


hid_t              hdf5_create_string_datatype(
                                        size_t const nr_bytes);

size_t             hdf5_nr_bytes       (hid_t const datatype);

herr_t             hdf5_close_datatype (hid_t const id);

hid_t              hdf5_datatype_property_list(
                                        hid_t const datatype_id);

H5T_cset_t         hdf5_datatype_encoding(
                                        hid_t const string_datatype_id);

htri_t             hdf5_is_int32_type  (hid_t const datatype);

htri_t             hdf5_is_uint64_type (hid_t const datatype);


#ifdef __cplusplus
}  // extern "C"
#endif
