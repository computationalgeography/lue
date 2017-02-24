#pragma once
#include <hdf5.h>


#ifdef __cplusplus
extern "C" {
#endif


hid_t              create_space_domain (hid_t const location_id);

htri_t             space_domain_exists (hid_t const location_id);

hid_t              open_space_domain   (hid_t const location_id);

herr_t             close_space_domain  (hid_t const domain_id);


#ifdef __cplusplus
}  // extern "C"
#endif
