#include "lue/c_api/dataset.h"
#include <assert.h>
#include "lue/c_api/phenomena.h"
#include "lue/c_api/universes.h"
#include "lue/c_api/hdf5/hdf5_file.h"
#include "lue/c_api/hdf5/hdf5_group.h"
#include "lue/c_api/hdf5/hdf5_identifier.h"


/*!
    @ingroup    lue_c_api_group
*/
hid_t create_dataset(
    char const* name)
{
    hid_t file_id = hdf5_create_file(name);

    if(file_id < 0) {
        return file_id;
    }


    // Create group for universes.
    hid_t universes_id = create_universes(file_id);

    if(universes_id < 0) {
        hdf5_close_group(file_id);
        return universes_id;
    }

    herr_t status = close_universes(universes_id);

    if(status < 0) {
        hdf5_close_group(file_id);
        return status;
    }


    // Create group for phenomena.
    hid_t phenomena_id = create_phenomena(file_id);

    if(phenomena_id < 0) {
        hdf5_close_group(file_id);
        return phenomena_id;
    }

    status = close_phenomena(phenomena_id);

    if(status < 0) {
        hdf5_close_group(file_id);
        return status;
    }


    return file_id;
}


/*!
    @ingroup    lue_c_api_group
*/
hid_t open_dataset(
    char const* name,
    unsigned int const flags)
{
    return hdf5_open_file(name, flags);
}


/*!
    @ingroup    lue_c_api_group
*/
herr_t close_dataset(
    hid_t const location_id)
{
    assert(hdf5_is_file(location_id));

    return hdf5_close_file(location_id);
}
