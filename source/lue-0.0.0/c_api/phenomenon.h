#pragma once
#include <hdf5.h>


#ifdef __cplusplus
extern "C" {
#endif


hid_t              create_phenomenon   (hid_t const location_id,
                                        char const* name);

hid_t              open_phenomenon     (hid_t const location_id,
                                        char const* name);

herr_t             close_phenomenon    (hid_t const id);

htri_t             phenomenon_exists   (hid_t const location_id,
                                        char const* name);

ssize_t            read_phenomenon_name(hid_t const location_id,
                                        char** const name);

htri_t             is_phenomenon       (hid_t const location_id);


#ifdef __cplusplus
}  // extern "C"
#endif
