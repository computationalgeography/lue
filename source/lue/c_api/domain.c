#include "lue/c_api/domain.h"
#include "lue/c_api/hdf5/hdf5_group.h"


static char const* domain_name = "domain";


/*!
    @ingroup    lue_c_api_group
*/
hid_t create_domain(
    hid_t const location_id)
{
    return hdf5_create_group(location_id, domain_name);
}


/*!
    @ingroup    lue_c_api_group
*/
htri_t domain_exists(
    hid_t const location_id)
{
    return hdf5_group_exists(location_id, domain_name);
}


/*!
    @ingroup    lue_c_api_group
*/
hid_t open_domain(
    hid_t const location_id)
{
    return hdf5_open_group(location_id, domain_name);
}


/*!
    @ingroup    lue_c_api_group
*/
herr_t close_domain(
    hid_t const domain_id)
{
    return hdf5_close_group(domain_id);
}
