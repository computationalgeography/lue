#include "lue/c_api/hdf5/hdf5_link.h"
#include "lue/c_api/hdf5/hdf5_identifier.h"
#include <stdlib.h>


/*!
    @ingroup    lue_c_api_hdf5_group
*/
htri_t hdf5_link_exists(
    hid_t const location_id,
    char const* name)
{
    return H5Lexists(location_id, name, H5P_DEFAULT);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
htri_t hdf5_link_is_group(
    hid_t const location_id,
    char const* name)
{
    htri_t result = -1;
    H5O_info_t info;

    herr_t status = H5Oget_info_by_name(location_id, name, &info,
        H5P_DEFAULT);

    if(status >= 0) {
        result = info.type == H5O_TYPE_GROUP ? 1 : 0;
    }

    return result;
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
htri_t hdf5_link_is_dataset(
    hid_t const location_id,
    char const* name)
{
    htri_t result = -1;
    H5O_info_t info;

    herr_t status = H5Oget_info_by_name(location_id, name, &info,
        H5P_DEFAULT);

    if(status >= 0) {
        result = info.type == H5O_TYPE_DATASET ? 1 : 0;
    }

    return result;
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
herr_t hdf5_create_hard_link(
    hid_t const target_id,
    char const* link_name,
    hid_t const location_id)
{
    // Determine path to object to link to.
    char* target_path;
    ssize_t length = hdf5_name(target_id, &target_path);

    if(length < 0) {
        return length;
    }


    // Create link from location_id/link_name to target_path.
    herr_t status = H5Lcreate_hard(target_id, target_path, location_id,
        link_name, H5P_DEFAULT, H5P_DEFAULT);
    free(target_path);

    if(status < 0) {
        return status;
    }


    return status;
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
herr_t hdf5_create_soft_link(
    hid_t const target_id,
    char const* link_name,
    hid_t const location_id)
{
    // Determine path to object to link to.
    char* target_path;
    ssize_t length = hdf5_name(target_id, &target_path);

    if(length < 0) {
        return length;
    }


    // Create link from location_id/link_name to target_path.
    herr_t status = H5Lcreate_soft(target_path, location_id, link_name,
        H5P_DEFAULT, H5P_DEFAULT);
    free(target_path);

    if(status < 0) {
        return status;
    }


    return status;
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
herr_t hdf5_delete_link(
    hid_t const location_id,
    char const* link_name)
{
    return H5Ldelete(location_id, link_name, H5P_DEFAULT);
}
