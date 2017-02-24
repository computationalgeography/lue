#include "lue/c_api/hdf5/hdf5_dataset.h"
#include "lue/c_api/hdf5/hdf5_link.h"


/*!
    @ingroup    lue_c_api_hdf5_group
*/
hid_t hdf5_create_dataset(
    hid_t const location_id,
    char const* name,
    hid_t const datatype_id,
    hid_t const dataspace_id)
{
    return H5Dcreate2(location_id, name, datatype_id, dataspace_id,
            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
htri_t hdf5_dataset_exists(
    hid_t const location_id,
    char const* name)
{
    htri_t status = hdf5_link_exists(location_id, name);

    if(status == 1) {
        status = hdf5_link_is_dataset(location_id, name);
    }

    return status;
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
hid_t hdf5_open_dataset(
    hid_t const location_id,
    char const* name)
{
    return H5Dopen(location_id, name, H5P_DEFAULT);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
herr_t hdf5_close_dataset(
    hid_t const id)
{
    return H5Dclose(id);
}
