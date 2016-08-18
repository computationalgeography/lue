#include "lue/c_api/time_domain.h"
#include "lue/c_api/hdf5/hdf5_group.h"


static char const* time_domain_name = "time";


/*!
    @ingroup    lue_c_api_group
*/
hid_t create_time_domain(
    hid_t const location_id)
{
    return hdf5_create_group(location_id, time_domain_name);
}


/*!
    @ingroup    lue_c_api_group
*/
htri_t time_domain_exists(
    hid_t const location_id)
{
    return hdf5_group_exists(location_id, time_domain_name);
}


/*!
    @ingroup    lue_c_api_group
*/
hid_t open_time_domain(
    hid_t const location_id)
{
    return hdf5_open_group(location_id, time_domain_name);
}


/*!
    @ingroup    lue_c_api_group
*/
herr_t close_time_domain(
    hid_t const domain_id)
{
    return hdf5_close_group(domain_id);
}
