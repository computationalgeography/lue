#include "lue/c_api/hdf5/hdf5_group.h"
#include "lue/c_api/hdf5/hdf5_link.h"
#include <stdlib.h>
#include <string.h>


/*!
    @ingroup    lue_c_api_hdf5_group
    @brief      Create a group named @a name.
    @param      location_id Identifier of file or group of group to create.
    @param      name Name of group to create.
    @return     Group identifier if successful; otherwise a negative value.
*/
hid_t hdf5_create_group(
    hid_t const location_id,
    char const* name)
{
    return H5Gcreate(location_id, name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
htri_t hdf5_group_exists(
    hid_t const location_id,
    char const* name)
{
    htri_t status = hdf5_link_exists(location_id, name);

    if(status == 1) {
        status = hdf5_link_is_group(location_id, name);
    }

    return status;
}


/*!
    @ingroup    lue_c_api_hdf5_group
    @brief      Open an existing group named @a name.
    @param      location_id Identifier of file or group of group to open.
    @param      name Name of group to open.
    @return     Group identifier if successful; otherwise a negative value.
*/
hid_t hdf5_open_group(
    hid_t const location_id,
    char const* name)
{
    return H5Gopen(location_id, name, H5P_DEFAULT);
}


// herr_t count_datasets(
//     hid_t loc_id,
//     char const* name,
//     H5L_info_t const* info,
//     void* data)
// {
//     hsize_t* count = (hsize_t*)data;
// 
//     H5O_info_t infobuf;
//     herr_t status = H5Oget_info_by_name(loc_id, name, &infobuf, H5P_DEFAULT);
// 
//     if(status < 0) {
//         return status;
//     }
// 
//     if(infobuf.type == H5O_TYPE_DATASET) {
//         ++(*count);
//     }
// 
//     return 0;
// }
// 
// 
// herr_t hdf5_nr_datasets(
//     hid_t location_id,
//     hsize_t* count)
// {
//     // Iterate over contents and return the number of datasets found.
//     *count = 0;
// 
//     return H5Literate(location_id, H5_INDEX_NAME, H5_ITER_NATIVE, NULL,
//         count_datasets, (void*)count);
// }


struct CountGroups
{
    size_t nr_groups;
};


herr_t count_groups(
    hid_t const loc_id,
    char const* name,
    H5L_info_t const* info,
    void* data)
{
    struct CountGroups* data_ = (struct CountGroups*)data;

    H5O_info_t infobuf;
    herr_t status = H5Oget_info_by_name(loc_id, name, &infobuf, H5P_DEFAULT);

    if(status < 0) {
        return status;
    }

    if(infobuf.type == H5O_TYPE_GROUP) {
        ++data_->nr_groups;
    }

    return 0;
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
size_t hdf5_nr_groups(
    hid_t const location_id)
{
    // Iterate over contents and return the number of groups found.
    struct CountGroups data;
    data.nr_groups = 0;

    H5Literate(location_id, H5_INDEX_NAME, H5_ITER_NATIVE, NULL,
        count_groups, (void*)&data);

    return data.nr_groups;
}


struct CountDatasets
{
    size_t nr_datasets;
};


herr_t count_datasets(
    hid_t const loc_id,
    char const* name,
    H5L_info_t const* info,
    void* data)
{
    struct CountDatasets* data_ = (struct CountDatasets*)data;

    H5O_info_t infobuf;
    herr_t status = H5Oget_info_by_name(loc_id, name, &infobuf, H5P_DEFAULT);

    if(status < 0) {
        return status;
    }

    if(infobuf.type == H5O_TYPE_DATASET) {
        ++data_->nr_datasets;
    }

    return 0;
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
size_t hdf5_nr_datasets(
    hid_t const location_id)
{
    // Iterate over contents and return the number of datasets found.
    struct CountDatasets data;
    data.nr_datasets = 0;

    H5Literate(location_id, H5_INDEX_NAME, H5_ITER_NATIVE, NULL,
        count_datasets, (void*)&data);

    return data.nr_datasets;
}


struct GroupNames
{
    size_t nr_names;
    char** names;
};


herr_t group_names(
    hid_t const loc_id,
    char const* name,
    H5L_info_t const* info,
    void* data)
{
    struct GroupNames* data_ = (struct GroupNames*)data;

    H5O_info_t infobuf;
    herr_t status = H5Oget_info_by_name(loc_id, name, &infobuf, H5P_DEFAULT);

    if(status < 0) {
        return status;
    }

    if(infobuf.type == H5O_TYPE_GROUP) {
        data_->names[data_->nr_names] =
            (char*)malloc(strlen(name) * sizeof(char));
        strcpy(data_->names[data_->nr_names], name);
        ++data_->nr_names;
    }

    return 0;
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
herr_t hdf5_group_names(
    hid_t const location_id,
    char** const names)
{
    // Iterate over contents and return the names of the groups found.
    // It is assumed that the names array is large enough to hold names
    // for all groups. Memory for the names themselves is allocated by the
    // function and must be freed by the caller.
    struct GroupNames data;
    data.names = names;
    data.nr_names = 0;

    H5Literate(location_id, H5_INDEX_NAME, H5_ITER_NATIVE, NULL,
        group_names, (void*)&data);

    return 1;
}


struct DatasetNames
{
    size_t nr_names;
    char** names;
};


herr_t dataset_names(
    hid_t const loc_id,
    char const* name,
    H5L_info_t const* info,
    void* data)
{
    struct DatasetNames* data_ = (struct DatasetNames*)data;

    H5O_info_t infobuf;
    herr_t status = H5Oget_info_by_name(loc_id, name, &infobuf, H5P_DEFAULT);

    if(status < 0) {
        return status;
    }

    if(infobuf.type == H5O_TYPE_DATASET) {
        data_->names[data_->nr_names] =
            (char*)malloc(strlen(name) * sizeof(char));
        strcpy(data_->names[data_->nr_names], name);
        ++data_->nr_names;
    }

    return 0;
}


/*!
    @ingroup    lue_c_api_hdf5_group
*/
herr_t hdf5_dataset_names(
    hid_t const location_id,
    char** const names)
{
    // Iterate over contents and return the names of the datasets found.
    // It is assumed that the names array is large enough to hold names
    // for all datasets. Memory for the names themselves is allocated by the
    // function and must be freed by the caller.
    struct DatasetNames data;
    data.names = names;
    data.nr_names = 0;

    H5Literate(location_id, H5_INDEX_NAME, H5_ITER_NATIVE, NULL,
        dataset_names, (void*)&data);

    return 1;
}


/*!
    @ingroup    lue_c_api_hdf5_group
    @brief      Close the group whose identifier is @a id.
    @param      id Identifier of group to close.
    @return     Non-negative value if successful; otherwise a negative value.
*/
herr_t hdf5_close_group(
    hid_t const id)
{
    return H5Gclose(id);
}
