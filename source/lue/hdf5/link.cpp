#include "lue/hdf5/link.h"
#include <hdf5.h>


namespace lue {
namespace hdf5 {

bool link_exists(
    Identifier const& id,
    std::string const& name)
{
    return ::H5Lexists(id, name.c_str(), H5P_DEFAULT) > 0;
}


bool soft_link_exists(
    Identifier const& id,
    std::string const& name)
{
    ::H5L_info_t info;
    auto const status = ::H5Lget_info(id, name.c_str(), &info, H5P_DEFAULT);

    return status >= 0 && info.type == H5L_TYPE_SOFT;
}


bool link_is_dataset(
    Identifier const& id,
    std::string const& name)
{
    ::H5O_info_t info;

    ::herr_t status = ::H5Oget_info_by_name(id, name.c_str(), &info,
        H5P_DEFAULT);

    return status >= 0 && info.type == H5O_TYPE_DATASET;
}


bool link_is_group(
    Identifier const& id,
    std::string const& name)
{
    ::H5O_info_t info;

    ::herr_t status = ::H5Oget_info_by_name(id, name.c_str(), &info,
        H5P_DEFAULT);

    return status >= 0 && info.type == H5O_TYPE_GROUP;
}

}  // namespace hdf5
}  // namespace lue
