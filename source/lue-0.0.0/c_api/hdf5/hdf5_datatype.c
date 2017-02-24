#include "lue/c_api/hdf5/hdf5_datatype.h"


/*!
    @ingroup    lue_c_api_hdf5_group
*/
hid_t hdf5_create_string_datatype(
    size_t const nr_bytes)
{
    hid_t datatype_id = H5Tcopy(H5T_C_S1);

    if(datatype_id >= 0) {
        herr_t status = H5Tset_cset(datatype_id, H5T_CSET_UTF8);

        if(status < 0) {
            H5Tclose(datatype_id);
            return status;
        }

        if(nr_bytes > 0) {
            status = H5Tset_size(datatype_id, nr_bytes);

            if(status < 0) {
                H5Tclose(datatype_id);
                return status;
            }
        }
    }

    return datatype_id;
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
size_t hdf5_nr_bytes(
    hid_t const datatype)
{
    return H5Tget_size(datatype);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
herr_t hdf5_close_datatype(
    hid_t const id)
{
    return H5Tclose(id);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
hid_t hdf5_datatype_property_list(
    hid_t const datatype_id)
{
    return H5Tget_create_plist(datatype_id);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
H5T_cset_t hdf5_datatype_encoding(
    hid_t const string_datatype_id)
{
    return H5Tget_cset(string_datatype_id);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
htri_t hdf5_is_int32_type(
    hid_t const datatype)
{
    H5T_class_t class = H5Tget_class(datatype);

    if(class == H5T_NO_CLASS) {
        return -1;
    }

    if(class != H5T_INTEGER) {
        return 0;
    }

    H5T_sign_t sign = H5Tget_sign(datatype);

    if(sign == H5T_SGN_ERROR) {
        return -1;
    }

    if(sign != H5T_SGN_2) {
        return 0;
    }

    size_t const size = H5Tget_size(datatype);

    if(size == 0) {
        return -1;
    }

    if(size != 4) {
        return 0;
    }

    return 1;
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
htri_t hdf5_is_uint64_type(
    hid_t const datatype)
{
    H5T_class_t class = H5Tget_class(datatype);

    if(class == H5T_NO_CLASS) {
        return -1;
    }

    if(class != H5T_INTEGER) {
        return 0;
    }

    H5T_sign_t sign = H5Tget_sign(datatype);

    if(sign == H5T_SGN_ERROR) {
        return -1;
    }

    if(sign != H5T_SGN_NONE) {
        return 0;
    }

    size_t const size = H5Tget_size(datatype);

    if(size == 0) {
        return -1;
    }

    if(size != 8) {
        return 0;
    }

    return 1;
}
