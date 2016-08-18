#include "lue/c_api/phenomena.h"
#include "lue/c_api/hdf5/hdf5_group.h"


static char const* name = "phenomena";


/*!
    @ingroup    lue_c_api_group
*/
hid_t create_phenomena(
    hid_t const location_id)
{
    return hdf5_create_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
*/
htri_t phenomena_exists(
    hid_t const location_id)
{
    return hdf5_group_exists(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
*/
hid_t open_phenomena(
    hid_t const location_id)
{
    return hdf5_open_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
*/
herr_t phenomenon_names(
    hid_t const phenomena_id,
    char** const names)
{
    return hdf5_group_names(phenomena_id, names);
}


/*!
    @ingroup    lue_c_api_group
*/
size_t nr_phenomena(
    hid_t const phenomena_id)
{
    return hdf5_nr_groups(phenomena_id);
}


/*!
    @ingroup    lue_c_api_group
*/
herr_t close_phenomena(
    hid_t const phenomena_id)
{
    return hdf5_close_group(phenomena_id);
}
