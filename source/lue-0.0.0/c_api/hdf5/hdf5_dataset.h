#pragma once
#include <hdf5.h>


#ifdef __cplusplus
extern "C" {
#endif


hid_t              hdf5_create_dataset (hid_t const location_id,
                                        char const* name,
                                        hid_t const datatype_id,
                                        hid_t const dataspace_id);

htri_t             hdf5_dataset_exists   (hid_t const location_id,
                                        char const* name);

hid_t              hdf5_open_dataset   (hid_t const location_id,
                                        char const* name);

herr_t             hdf5_close_dataset    (hid_t const id);


#ifdef __cplusplus
}  // extern "C"
#endif
