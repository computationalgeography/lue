#pragma once
#include <hdf5.h>


/*!
    @file
    @ingroup    lue_c_api_hdf5_group
    @sa         https://www.hdfgroup.org/HDF5/doc/RM/RM_H5S.html,
*/


#ifdef __cplusplus
extern "C" {
#endif


// hid_t              hdf5_create_scalar_dataspace();

hid_t              hdf5_create_dataspace(
                                        hsize_t const size);

herr_t             hdf5_extent_dimensions(
                                        hid_t const dataspace_id,
                                        hsize_t* dimensions);

herr_t             hdf5_close_dataspace(hid_t const id);


#ifdef __cplusplus
}  // extern "C"
#endif
