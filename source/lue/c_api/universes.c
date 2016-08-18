#include "lue/c_api/universes.h"
#include "lue/c_api/hdf5/hdf5_group.h"


static char const* name = "universes";


/*!
    @ingroup    lue_c_api_group
    @brief      Create a group for multiple universes
    @param      location_id Identifier of file or group of group to create
    @return     Universes group identifier if successful; otherwise a
                negative value
*/
hid_t create_universes(
    hid_t const location_id)
{
    return hdf5_create_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Return whether or not a universes group exists
    @param      location_id Idenfitief of file or group of group to test
    @todo       Make this function return a `htri_t` (see @ref lue_hdf5)
*/
htri_t universes_exists(
    hid_t const location_id)
{
    return hdf5_group_exists(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Open an existing universes group
    @param      location_id Identifier of file or group of group to open
    @return     Universes group identifier if successful; otherwise a
                negative value
*/
hid_t open_universes(
    hid_t const location_id)
{
    return hdf5_open_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Obtain the names of the universes
    @param      universes_id Identifier of universes group
    @param      names Array of pointers for storing the names
    @sa         nr_universes()

    The @a names array must be large enough to hold all names. Memory fo the
    names themselves is allocated by the function and must be freed by the
    caller (using `free`).
*/
herr_t universe_names(
    hid_t const universes_id,
    char** const names)
{
    return hdf5_group_names(universes_id, names);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Return the number of universes
    @param      universes_id Identifier of universes group
*/
size_t nr_universes(
    hid_t const universes_id)
{
    return hdf5_nr_groups(universes_id);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Close the universes group.
    @param      universes_id Identifier of universes group
*/
herr_t close_universes(
    hid_t const universes_id)
{
    return hdf5_close_group(universes_id);
}
