#pragma once
#include <hdf5.h>


#ifdef __cplusplus
extern "C" {
#endif


hid_t              create_collection   (hid_t const location_id,
                                        char const* name);

htri_t             collection_exists   (hid_t const location_id,
                                        char const* name);

hid_t              open_collection     (hid_t const location_id,
                                        char const* name);

size_t             nr_items            (hid_t const collection_id);

herr_t             item_names          (hid_t const collection_id,
                                        char** const names);

herr_t             close_collection    (hid_t const collection_id);


#ifdef __cplusplus
}  // extern "C"
#endif
