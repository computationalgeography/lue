#pragma once
#include <hdf5.h>


#ifdef __cplusplus
extern "C" {
#endif


hid_t              create_properties   (hid_t const location_id);

htri_t             properties_exists   (hid_t const location_id);

hid_t              open_properties     (hid_t const location_id);

size_t             nr_properties       (hid_t const properties_id);

herr_t             property_names      (hid_t const properties_id,
                                        char** const names);

herr_t             close_properties    (hid_t const properties_id);


#ifdef __cplusplus
}  // extern "C"
#endif
