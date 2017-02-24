#pragma once
#include <hdf5.h>


#ifdef __cplusplus
extern "C" {
#endif


htri_t             hdf5_link_exists    (hid_t const location_id,
                                        char const* name);

htri_t             hdf5_link_is_group  (hid_t const location_id,
                                        char const* name);

htri_t             hdf5_link_is_dataset(hid_t const location_id,
                                        char const* name);

herr_t             hdf5_create_hard_link(
                                        hid_t const target_id,
                                        char const* link_name,
                                        hid_t const location_id);

herr_t             hdf5_create_soft_link(
                                        hid_t const target_id,
                                        char const* link_name,
                                        hid_t const location_id);

herr_t             hdf5_delete_link    (hid_t const location_id,
                                        char const* link_name);


#ifdef __cplusplus
}  // extern "C"
#endif
