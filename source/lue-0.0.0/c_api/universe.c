#include "lue/c_api/universe.h"
#include "lue/c_api/phenomena.h"
#include "lue/c_api/hdf5/hdf5_group.h"
#include "lue/c_api/hdf5/hdf5_identifier.h"


/*!
    @ingroup    lue_c_api_group
    @brief      Create a new universe named @a name.
    @param      location_id Identifier of file or group of universe to create.
    @param      name Name of universe to create.
    @return     Universe identifier if successful; otherwise a
                negative value.
*/
hid_t create_universe(
    hid_t const location_id,
    char const* name)
{
    // Create new hdf group for the universe in the location passed in.
    hid_t universe_id = hdf5_create_group(location_id, name);

    if(universe_id < 0) {
        return universe_id;
    }


    // Create group for phenomena.
    hid_t phenomena_id = create_phenomena(universe_id);

    if(phenomena_id < 0) {
        hdf5_close_group(universe_id);
        return phenomena_id;
    }

    herr_t status = close_phenomena(phenomena_id);

    if(status < 0) {
        hdf5_close_group(universe_id);
        return status;
    }


    return universe_id;
}


/*!
    @ingroup    lue_c_api_group
    @brief      Open an existing universe named @a name.
    @param      location_id Identifier of file or group of universe to open.
    @param      name Name of universe to open.
    @return     Universe identifier if successful; otherwise a
                negative value.
*/
hid_t open_universe(
    hid_t const location_id,
    char const* name)
{
    return hdf5_open_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Close the universe whose identifier is @a id.
    @param      id Identifier of universe to close.
    @return     Non-negative value if successful; otherwise a negative value.
*/
herr_t close_universe(
    hid_t const id)
{
    return hdf5_close_group(id);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Return whether a universe exists.
    @param      location_id Identifier of file or group of universe.
    @param      name Name of universe to test.
*/
htri_t universe_exists(
    hid_t const location_id,
    char const* name)
{
    return hdf5_group_exists(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Read the name of the universe.
    @param      location_id Identifier of universe.
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
ssize_t read_universe_name(
    hid_t const location_id,
    char** const name)
{
    return hdf5_name(location_id, name);
}
