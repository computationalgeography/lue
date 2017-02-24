#pragma once
#include <hdf5.h>


#ifdef __cplusplus
extern "C" {
#endif


hid_t              create_property_sets(hid_t const location_id);

htri_t             property_sets_exists(hid_t const location_id);

hid_t              open_property_sets  (hid_t const location_id);

size_t             nr_property_sets    (hid_t const property_sets_id);

herr_t             property_set_names  (hid_t property_sets_id,
                                        char** const names);

herr_t             close_property_sets (hid_t const property_sets_id);


#ifdef __cplusplus
}  // extern "C"
#endif
