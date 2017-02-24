#include "lue/cxx_api/hdf5/link.h"
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
    H5L_info_t info;
    auto const status = ::H5Lget_info(id, name.c_str(), &info, H5P_DEFAULT);

    return status >= 0 && info.type == H5L_TYPE_SOFT;
}

}  // namespace hdf5
}  // namespace lue
