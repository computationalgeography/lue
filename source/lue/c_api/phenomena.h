#pragma once
#include <hdf5.h>


#ifdef __cplusplus
extern "C" {
#endif


hid_t              create_phenomena    (hid_t const location_id);

htri_t             phenomena_exists    (hid_t const location_id);

hid_t              open_phenomena      (hid_t const location_id);

size_t             nr_phenomena        (hid_t const phenomena_id);

herr_t             phenomenon_names    (hid_t const phenomena_id,
                                        char** const names);

herr_t             close_phenomena     (hid_t const phenomena_id);


#ifdef __cplusplus
}  // extern "C"
#endif
