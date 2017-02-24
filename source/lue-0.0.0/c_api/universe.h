#pragma once
#include <hdf5.h>


#ifdef __cplusplus
extern "C" {
#endif


hid_t              create_universe     (hid_t const location_id,
                                        char const* name);

hid_t              open_universe       (hid_t const location_id,
                                        char const* name);

herr_t             close_universe      (hid_t const id);

htri_t             universe_exists     (hid_t const location_id,
                                        char const* name);

ssize_t            read_universe_name  (hid_t const location_id,
                                        char** const name);


#ifdef __cplusplus
}  // extern "C"
#endif
