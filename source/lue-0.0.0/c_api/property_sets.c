#include "lue/c_api/property_sets.h"
#include "lue/c_api/hdf5/hdf5_group.h"


static char const* name = "lue_property_sets";


/*!
    @ingroup    lue_c_api_group
*/
hid_t create_property_sets(
    hid_t const location_id)
{
    return hdf5_create_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
*/
htri_t property_sets_exists(
    hid_t const location_id)
{
    return hdf5_group_exists(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
*/
hid_t open_property_sets(
    hid_t const location_id)
{
    return hdf5_open_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
*/
herr_t property_set_names(
    hid_t const property_sets_id,
    char** const names)
{
    return hdf5_group_names(property_sets_id, names);
}


/*!
    @ingroup    lue_c_api_group
*/
size_t nr_property_sets(
    hid_t const property_sets_id)
{
    return hdf5_nr_groups(property_sets_id);
}


/*!
    @ingroup    lue_c_api_group
*/
herr_t close_property_sets(
    hid_t const property_sets_id)
{
    return hdf5_close_group(property_sets_id);
}
