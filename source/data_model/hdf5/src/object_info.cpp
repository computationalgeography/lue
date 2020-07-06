#include "lue/hdf5/object_info.hpp"
#include <stdexcept>


namespace lue {
namespace hdf5 {

ObjectInfo::ObjectInfo(
    hid_t const id):

    _info{}

{
    auto status = ::H5Oget_info(id, &_info);

    if(status < 0) {
        throw std::runtime_error("Cannot retrieve object info");
    }
}


unsigned long ObjectInfo::fileno() const  // NOLINT(google-runtime-int)
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
bool ObjectInfo::operator==(
    ObjectInfo const& other) const
{
    return
        fileno() == other.fileno() &&
        addr() == other.addr();
}


/*!
    @brief      Return whether two instances are not equal
    @sa         operator==(ObjectInfo const&) const

    Two instances are considered equal if they are pointing to the same
    object in the HDF5 dataset.
*/
bool ObjectInfo::operator!=(
    ObjectInfo const& other) const
{
    return !(*this == other);
}

}  // namespace hdf5
}  // namespace lue
