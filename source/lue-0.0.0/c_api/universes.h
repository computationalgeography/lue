#pragma once
#include <hdf5.h>


#ifdef __cplusplus
extern "C" {
#endif


hid_t              create_universes    (hid_t const location_id);

htri_t             universes_exists    (hid_t const location_id);

hid_t              open_universes      (hid_t const location_id);

size_t             nr_universes        (hid_t const universes_id);

herr_t             universe_names      (hid_t const universes_id,
                                        char** const names);

herr_t             close_universes     (hid_t const universes_id);


#ifdef __cplusplus
}  // extern "C"
#endif
