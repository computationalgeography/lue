#include "lue/c_api/domain.h"
#include "lue/c_api/space_domain.h"
#include "lue/c_api/time_domain.h"
#include "lue/c_api/hdf5/hdf5_group.h"


static char const* domain_name = "domain";


/*!
    @ingroup    lue_c_api_group
*/
hid_t create_domain(
    hid_t const location_id)
{
    // Create group for domain.
    hid_t domain_id = hdf5_create_group(location_id, domain_name);

    if(domain_id < 0) {
        return domain_id;
    }


    // // Create group for time domain.
    // hid_t time_domain_id = create_time_domain(domain_id);

    // if(time_domain_id < 0) {
    //     hdf5_close_group(domain_id);
    //     return time_domain_id;
    // }

    // herr_t status = close_time_domain(time_domain_id);

    // if(status < 0) {
    //     hdf5_close_group(domain_id);
    //     return status;
    // }


    // // Create group for space domain.
    // hid_t space_domain_id = create_space_domain(domain_id);

    // if(space_domain_id < 0) {
    //     hdf5_close_group(domain_id);
    //     return space_domain_id;
    // }

    // status = close_space_domain(space_domain_id);

    // if(status < 0) {
    //     hdf5_close_group(domain_id);
    //     return status;
    // }


    return domain_id;
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
