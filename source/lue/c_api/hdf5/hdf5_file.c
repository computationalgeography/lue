#include "lue/c_api/hdf5/hdf5_file.h"
#include "lue/c_api/hdf5/hdf5_identifier.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>


/*!
    @ingroup    lue_c_api_hdf5_group
    @brief      Create a new file named @a name.
    @param      name Name of file to create, encoded in UTF8.
    @return     File identifier if successful; otherwise a negative value.

    If the file already exists, it is truncated first. The file is created
    in read-write mode.
*/
hid_t hdf5_create_file(
    char const* name)
{
    return H5Fcreate(name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
}


/*!
    @ingroup    lue_c_api_hdf5_group
    @brief      Open an existing file named @a name.
    @param      name Name of file to open, encoded in UTF8.
    @param      flags File access flags. Allowable values are `H5F_ACC_RDWR`
                and `H5F_ACC_RDONLY`.
    @return     File identifier if successful; otherwise a negative value.
*/
hid_t hdf5_open_file(
    char const* name,
    unsigned int const flags)
{
    return H5Fopen(name, flags, H5P_DEFAULT);
}


/*!
    @ingroup    lue_c_api_hdf5_group
    @brief      Close the file whose identifier is @a id.
    @param      file_id Identifier of file to close.
    @return     Non-negative value if successful; otherwise a negative value.
*/
herr_t hdf5_close_file(
    hid_t const file_id)
{
    assert(hdf5_is_file(file_id));

#ifndef NDEBUG
    // Determine name of file pointed to by file_id.
    ssize_t nr_bytes = H5Fget_name(file_id, NULL, 0);
    assert(nr_bytes > 0);
    char name[(nr_bytes + 1) * sizeof(char)];
    nr_bytes = H5Fget_name(file_id, name, nr_bytes + 1);
    assert(nr_bytes > 0);

    // Try to close the file.
    herr_t status = H5Fclose(file_id);

    if(status < 0) {
        return status;
    }

    // In case there are still open references to stuff in the file,
    // closing the file results in a corrupt hdf5 file. Lets verify this
    // isn't the case.
    assert(hdf5_is_hdf5_file(name));

    return status;
#else
    return H5Fclose(file_id);
#endif
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
bool hdf5_is_hdf5_file(
    char const* name)
{
    return H5Fis_hdf5(name) > 0;
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
ssize_t hdf5_file_pathname(
    hid_t const id,
    char** name)
{
    ssize_t nr_bytes = H5Fget_name(id, NULL, 0);

    if(nr_bytes > 0) {
        *name = (char*)malloc((nr_bytes + 1) * sizeof(char));
        /* nr_bytes = */ H5Fget_name(id, *name, nr_bytes + 1);
        assert((*name)[nr_bytes] == '\0');
        assert(strlen(*name) == (size_t)nr_bytes);
    }

    return nr_bytes;
}
