#include "lue/c_api/hdf5/hdf5_identifier.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>


/*!
    @ingroup    lue_c_api_hdf5_group
*/
htri_t hdf5_is_valid(
    hid_t const id)
{
    return H5Iis_valid(id);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
htri_t hdf5_is_file(
    hid_t const id)
{
    H5I_type_t type = H5Iget_type(id);

    return type == H5I_BADID ? -1 : (type == H5I_FILE ? 1 : 0);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
ssize_t hdf5_name(
    hid_t const id,
    char** name)
{
    ssize_t nr_bytes = H5Iget_name(id, NULL, 0);

    if(nr_bytes > 0) {
        *name = (char*)malloc((nr_bytes + 1) * sizeof(char));
        /* nr_bytes = */ H5Iget_name(id, *name, nr_bytes + 1);
        assert((*name)[nr_bytes] == '\0');
        assert(strlen(*name) == (size_t)nr_bytes);
    }

    return nr_bytes;
}
