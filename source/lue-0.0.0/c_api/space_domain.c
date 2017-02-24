#include "lue/c_api/space_domain.h"
#include "lue/c_api/hdf5/hdf5_group.h"


static char const* space_domain_name = "lue_space";


/*!
    @ingroup    lue_c_api_group
*/
hid_t create_space_domain(
    hid_t const location_id)
{
    return hdf5_create_group(location_id, space_domain_name);
}


/*!
    @ingroup    lue_c_api_group
*/
htri_t space_domain_exists(
    hid_t const location_id)
{
    return hdf5_group_exists(location_id, space_domain_name);
}


/*!
    @ingroup    lue_c_api_group
*/
hid_t open_space_domain(
    hid_t const location_id)
{
    return hdf5_open_group(location_id, space_domain_name);
}


/*!
    @ingroup    lue_c_api_group
*/
herr_t close_space_domain(
    hid_t const domain_id)
{
    return hdf5_close_group(domain_id);
}
