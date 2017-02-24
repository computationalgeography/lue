#include "lue/c_api/properties.h"
#include "lue/c_api/hdf5/hdf5_group.h"


static char const* name = "lue_properties";


/*!
    @ingroup    lue_c_api_group
    @brief      Create a group for multiple properties
    @param      location_id Identifier of file or group of group to create
    @return     Properties group identifier if successful; otherwise a
                negative value
*/
hid_t create_properties(
    hid_t const location_id)
{
    return hdf5_create_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Return whether or not a properties group exists
    @param      location_id Idenfitief of file or group of group to test
    @todo       Make this function return a `htri_t` (see @ref lue_hdf5)
*/
htri_t properties_exists(
    hid_t const location_id)
{
    return hdf5_group_exists(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Open an existing properties group
    @param      location_id Identifier of file or group of group to open
    @return     Properties group identifier if successful; otherwise a
                negative value
*/
hid_t open_properties(
    hid_t const location_id)
{
    return hdf5_open_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Obtain the names of the properties
    @param      properties_id Identifier of properties group
    @param      names Array of pointers for storing the names
    @sa         nr_properties()

    The @a names array must be large enough to hold all names. Memory fo the
    names themselves is allocated by the function and must be freed by the
    caller (using `free`).
*/
herr_t property_names(
    hid_t const properties_id,
    char** const names)
{
    return hdf5_group_names(properties_id, names);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Return the number of properties
    @param      properties_id Identifier of properties group
*/
size_t nr_properties(
    hid_t const properties_id)
{
    return hdf5_nr_groups(properties_id);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Close the properties group.
    @param      properties_id Identifier of properties group
*/
herr_t close_properties(
    hid_t const properties_id)
{
    return hdf5_close_group(properties_id);
}
