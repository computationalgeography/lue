#include "lue/c_api/phenomenon.h"
#include "lue/c_api/property_sets.h"
#include "lue/c_api/hdf5/hdf5_group.h"
#include "lue/c_api/hdf5/hdf5_identifier.h"


/*!
    @ingroup    lue_c_api_group
    @brief      Create a new phenomenon named @a name.
    @param      location_id Identifier of file or group of phenomenon to
                create.
    @param      name Name of phenomenon to create.
    @return     Universe identifier if successful; otherwise a
                negative value.
*/
hid_t create_phenomenon(
    hid_t const location_id,
    char const* name)
{
    // Create new hdf group for the phenomenon in the location passed in.
    hid_t phenomenon_id = hdf5_create_group(location_id, name);

    if(phenomenon_id < 0) {
        return phenomenon_id;
    }


    // Create group for property sets.
    hid_t property_sets_id = create_property_sets(phenomenon_id);

    if(property_sets_id < 0) {
        hdf5_close_group(phenomenon_id);
        return property_sets_id;
    }

    herr_t status = close_property_sets(property_sets_id);

    if(status < 0) {
        hdf5_close_group(phenomenon_id);
        return status;
    }


    return phenomenon_id;
}


/*!
    @ingroup    lue_c_api_group
    @brief      Open an existing phenomenon named @a name.
    @param      location_id Identifier of file or group of phenomenon to open.
    @param      name Name of phenomenon to open.
    @return     Phenomenon identifier if successful; otherwise a
                negative value.
*/
hid_t open_phenomenon(
    hid_t const location_id,
    char const* name)
{
    return hdf5_open_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Return whether a phenomenon exists.
    @param      location_id Identifier of file or group of phenomenon.
    @param      name Name of phenomenon to test.
*/
htri_t phenomenon_exists(
    hid_t const location_id,
    char const* name)
{
    return hdf5_group_exists(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Close the phenomenon whose identifier is @a id.
    @param      id Identifier of phenomenon to close.
    @return     Non-negative value if successful; otherwise a negative value.
*/
herr_t close_phenomenon(
    hid_t const id)
{
    return hdf5_close_group(id);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Read the name of the phenomenon.
    @param      location_id Identifier of phenomenon.
    @param      name Pointer to location to store pointer to the array
                of characters for the name.
    @return     The length of the name if successful, returning 0 (zero)
                if no name is associated with the identifier. Otherwise
                returns a negative value.

    Memory for the array of characters will be allocated. The caller is
    responsible for deallocating it (using free).

    The length of the name returned does not include the terminating
    null character.
*/
ssize_t read_phenomenon_name(
    hid_t const location_id,
    char** const name)
{
    return hdf5_name(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
*/
htri_t is_phenomenon(
    hid_t const location_id)
{
    return property_sets_exists(location_id);
}
