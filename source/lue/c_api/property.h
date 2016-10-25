#pragma once
#include <hdf5.h>


#ifdef __cplusplus
extern "C" {
#endif


hid_t              create_property     (hid_t const location_id,
                                        char const* name,
                                        hid_t const domain_id);

hid_t              open_property       (hid_t const location_id,
                                        char const* name);

herr_t             close_property      (hid_t const id);

htri_t             property_exists     (hid_t const location_id,
                                        char const* name);

ssize_t            read_property_name  (hid_t const location_id,
                                        char** const name);


#ifdef __cplusplus
}  // extern "C"
#endif
