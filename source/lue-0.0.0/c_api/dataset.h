#pragma once
#include <hdf5.h>


#ifdef __cplusplus
extern "C" {
#endif


hid_t              create_dataset      (char const* name);

hid_t              open_dataset        (char const* name,
                                        unsigned int const flags);

herr_t             close_dataset       (hid_t const location_id);


#ifdef __cplusplus
}  // extern "C"
#endif
