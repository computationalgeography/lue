#include "lue/c_api/value.h"
#include "lue/c_api/hdf5/hdf5_group.h"


static char const* name = "value";


/*!
    @ingroup    lue_c_api_group
*/
hid_t create_value(
    hid_t const location_id)
{
    return hdf5_create_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
*/
htri_t value_exists(
    hid_t const location_id)
{
    return hdf5_group_exists(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
*/
hid_t open_value(
    hid_t const location_id)
{
    return hdf5_open_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
*/
herr_t close_value(
    hid_t const value_id)
{
    return hdf5_close_group(value_id);
}
