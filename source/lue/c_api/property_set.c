#include "lue/c_api/property_set.h"
#include "lue/c_api/domain.h"
#include "lue/c_api/properties.h"
#include "lue/c_api/hdf5/hdf5_group.h"
#include "lue/c_api/hdf5/hdf5_identifier.h"


/*!
    @ingroup    lue_c_api_group
    @brief      Create a new property set named @a name.
    @param      location_id Identifier of file or group of property set to
                create.
    @param      name Name of property set to create.
    @return     Property set identifier if successful; otherwise a
                negative value.
*/
hid_t create_property_set(
    hid_t const location_id,
    char const* name)
{
    // Create new hdf group for the property set in the location passed in.
    hid_t property_set_id = hdf5_create_group(location_id, name);

    if(property_set_id < 0) {
        return property_set_id;
    }


    // Create group for domain.
    hid_t domain_id = create_domain(property_set_id);

    if(domain_id < 0) {
        hdf5_close_group(property_set_id);
        return domain_id;
    }

    herr_t status = close_domain(domain_id);

    if(status < 0) {
        hdf5_close_group(property_set_id);
        return status;
    }


    // Create group for properties.
    hid_t properties_id = create_properties(property_set_id);

    if(properties_id < 0) {
        hdf5_close_group(property_set_id);
        return properties_id;
    }

    status = close_properties(properties_id);

    if(status < 0) {
        hdf5_close_group(property_set_id);
        return status;
    }


    return property_set_id;
}


/*!
    @ingroup    lue_c_api_group
    @brief      Open an existing property set named @a name.
    @param      location_id Identifier of file or group of property set
                to open.
    @param      name Name of property set to open.
    @return     Property set identifier if successful; otherwise a
                negative value.
*/
hid_t open_property_set(
    hid_t const location_id,
    char const* name)
{
    return hdf5_open_group(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Return whether a property set exists.
    @param      location_id Identifier of file or group of property set.
    @param      name Name of property set to test.
*/
htri_t property_set_exists(
    hid_t const location_id,
    char const* name)
{
    return hdf5_group_exists(location_id, name);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Close the property set whose identifier is @a id.
    @param      id Identifier of property set to close.
    @return     Non-negative value if successful; otherwise a negative value.
*/
herr_t close_property_set(
    hid_t const id)
{
    return hdf5_close_group(id);
}


/*!
    @ingroup    lue_c_api_group
    @brief      Read the name of the property set.
    @param      location_id Identifier of property set.
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
ssize_t read_property_set_name(
    hid_t const location_id,
    char** const name)
{
    return hdf5_name(location_id, name);
}
