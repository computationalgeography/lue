#pragma once
#include <hdf5.h>


/*!
    @file
    @ingroup    lue_c_api_hdf5_group
    @sa         - https://www.hdfgroup.org/HDF5/doc/RM/RM_H5I.html
*/


#ifdef __cplusplus
extern "C" {
#endif


htri_t             hdf5_is_valid       (hid_t const id);

htri_t             hdf5_is_file        (hid_t const id);

ssize_t            hdf5_name           (hid_t const id,
                                        char** name);


#ifdef __cplusplus
}  // extern "C"
#endif
