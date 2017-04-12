#include "lue/hdf5/object_info.h"
#include <stdexcept>


namespace lue {
namespace hdf5 {

ObjectInfo::ObjectInfo(
    hid_t const id)
{
    auto status = ::H5Oget_info(id, &_info);

    if(status < 0) {
        throw std::runtime_error("Cannot retrieve object info");
    }
}


unsigned long ObjectInfo::fileno() const
{
    return _info.fileno;
}


haddr_t ObjectInfo::addr() const
{
    return _info.addr;
}


/*!
    @brief      Return whether two instances are equal

    Two instances are considered equal if they are pointing to the same
    object in the HDF5 dataset.
*/
bool operator==(
    ObjectInfo const& lhs,
    ObjectInfo const& rhs)
{
    return
        lhs.fileno() == rhs.fileno() &&
        lhs.addr() == rhs.addr()
        ;
}


/*!
    @brief      Return whether two instances are not equal
    @sa         operator==(ObjectInfo const&, ObjectInfo const&)
*/
bool operator!=(
    ObjectInfo const& lhs,
    ObjectInfo const& rhs)
{
    return !(lhs == rhs);
}


}  // namespace hdf5
}  // namespace lue
