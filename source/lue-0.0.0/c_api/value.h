#pragma once
#include <hdf5.h>


#ifdef __cplusplus
extern "C" {
#endif


hid_t              create_value        (hid_t const location_id);

htri_t             value_exists        (hid_t const location_id);

hid_t              open_value          (hid_t const location_id);

herr_t             close_value         (hid_t const value_id);


#ifdef __cplusplus
}  // extern "C"
#endif
