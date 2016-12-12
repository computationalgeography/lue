#pragma once
#include <hdf5.h>


#ifdef __cplusplus
extern "C" {
#endif


hid_t              hdf5_create_group   (hid_t const location_id,
                                        char const* name);

htri_t             hdf5_group_exists   (hid_t const location_id,
                                        char const* name);

hid_t              hdf5_open_group     (hid_t const location_id,
                                        char const* name);

size_t             hdf5_nr_objects     (hid_t const location_id);

herr_t             hdf5_object_names   (hid_t const location_id,
                                        char** const names);

size_t             hdf5_nr_groups      (hid_t const location_id);

herr_t             hdf5_group_names    (hid_t const location_id,
                                        char** const names);

size_t             hdf5_nr_datasets    (hid_t const location_id);

herr_t             hdf5_dataset_names  (hid_t const location_id,
                                        char** const names);

herr_t             hdf5_close_group    (hid_t const id);


#ifdef __cplusplus
}  // extern "C"
#endif
