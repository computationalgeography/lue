#pragma once
#include <stdbool.h>
#include <hdf5.h>
#include "lue/configure.h"


/*!
    @file
    @ingroup    lue_c_api_hdf5_group
    @sa         - https://www.hdfgroup.org/HDF5/doc/RM/RM_H5F.html
*/


#ifdef __cplusplus
extern "C" {
#endif

hid_t              hdf5_create_file    (char const* name);

hid_t              hdf5_open_file      (char const* name,
                                        unsigned int const flags);

herr_t             hdf5_close_file     (hid_t const file_id);

bool               hdf5_is_hdf5_file   (char const* name);


#ifdef __cplusplus
}  // extern "C"
#endif
