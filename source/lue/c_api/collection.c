#include "lue/c_api/collection.h"
#include "lue/c_api/hdf5/hdf5_group.h"


// static char const* name = "collection";


/*!
    @ingroup    lue_c_api_group
    @brief      Create a group for multiple items
    @param      location_id Identifier of file or group of group to create
    @return     Collection group identifier if successful; otherwise a
                negative value
*/
hid_t create_collection(
    hid_t const location_id,
    char const* name)
{
    return hdf5_create_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Return whether or not a collection group exists
    @param      location_id Identifier of file or group of group to test
    @todo       Make this function return a `htri_t` (see @ref lue_hdf5)
*/
htri_t collection_exists(
    hid_t const location_id,
    char const* name)
{
    return hdf5_group_exists(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Open an existing collection group
    @param      location_id Identifier of file or group of group to open
    @return     Collection group identifier if successful; otherwise a
                negative value
*/
hid_t open_collection(
    hid_t const location_id,
    char const* name)
{
    return hdf5_open_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Obtain the names of the items
    @param      collection_id Identifier of collection group
    @param      names Array of pointers for storing the names
    @sa         nr_collection()

    The @a names array must be large enough to hold all names. Memory fo the
    names themselves is allocated by the function and must be freed by the
    caller (using `free`).
*/
herr_t item_names(
    hid_t const collection_id,
    char** const names)
{
    return hdf5_group_names(collection_id, names);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Return the number of items
    @param      collection_id Identifier of collection group
*/
size_t nr_items(
    hid_t const collection_id)
{
    return hdf5_nr_groups(collection_id);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Close the collection group.
    @param      collection_id Identifier of collection group
*/
herr_t close_collection(
    hid_t const collection_id)
{
    return hdf5_close_group(collection_id);
}
