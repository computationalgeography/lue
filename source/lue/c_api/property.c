#include "lue/c_api/property.h"
#include "lue/c_api/hdf5/hdf5_group.h"
#include "lue/c_api/hdf5/hdf5_identifier.h"
#include "lue/c_api/hdf5/hdf5_link.h"


/*!
    @ingroup    lue_c_api_group
    @brief      Create a new property named @a name.
    @param      location_id Identifier of file or group of property to
                create.
    @param      name Name of property to create.
    @return     Property identifier if successful; otherwise a
                negative value.
*/
hid_t create_property(
    hid_t const location_id,
    char const* name,
    hid_t const domain_id)
{
    hid_t group_id = hdf5_create_group(location_id, name);

    if(group_id < 0) {
        return group_id;
    }

    herr_t status = hdf5_create_soft_link(domain_id, "lue_domain", group_id);

    if(status < 0) {
        return status;
    }

    return group_id;
}


/*!
    @ingroup    lue_c_api_group
    @brief      Open an existing property named @a name.
    @param      location_id Identifier of file or group of property
                to open.
    @param      name Name of property to open.
    @return     Property identifier if successful; otherwise a
                negative value.
*/
hid_t open_property(
    hid_t const location_id,
    char const* name)
{
    return hdf5_open_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Return whether a property exists.
    @param      location_id Identifier of file or group of property.
    @param      name Name of property to test.
*/
htri_t property_exists(
    hid_t const location_id,
    char const* name)
{
    return hdf5_group_exists(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Close the property whose identifier is @a id.
    @param      id Identifier of property to close.
    @return     Non-negative value if successful; otherwise a negative value.
*/
herr_t close_property(
    hid_t const id)
{
    return hdf5_close_group(id);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Read the name of the property.
    @param      location_id Identifier of property.
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
ssize_t read_property_name(
    hid_t const location_id,
    char** const name)
{
    return hdf5_name(location_id, name);
}
