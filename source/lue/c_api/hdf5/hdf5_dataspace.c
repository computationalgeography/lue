#include "lue/c_api/hdf5/hdf5_dataspace.h"


// /*!
//     @ingroup    lue_c_api_hdf5_group
// */
// hid_t hdf5_create_scalar_dataspace()
// {
//     int const rank = 1;
//     hsize_t dimensions[1] = {H5S_UNLIMITED};
// 
//     return H5Screate_simple(rank, dimensions, NULL);
// 
//     // return H5Screate(H5S_SCALAR);
// }


/*!
    @ingroup    lue_c_api_hdf5_group
    @brief      Create a new 1-D dataspace for @a size elements.
    @param      size Size of the one dimension in the dataspace.
    @return     Dataspace identifier if successful; otherwise a negative value.
*/
hid_t hdf5_create_dataspace(
    hsize_t const size)
{
    hsize_t dimensions[1];
    dimensions[0] = size;

    return H5Screate_simple(1, dimensions, NULL);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
herr_t hdf5_extent_dimensions(
    hid_t const dataspace_id,
    hsize_t* dimensions)
{
    return H5Sget_simple_extent_dims(dataspace_id, dimensions, NULL);
}


/*!
    @ingroup    lue_c_api_hdf5_group
    @brief      Close the dataspace whose identifier is @a id.
    @param      id Identifier of dataspace to close.
    @return     Non-negative value if successful; otherwise a negative value.
*/
herr_t hdf5_close_dataspace(
    hid_t const id)
{
    return H5Sclose(id);
}
