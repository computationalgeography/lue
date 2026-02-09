#include "lue/hdf5/object_info.hpp"
#include <stdexcept>


namespace lue::hdf5 {

    ObjectInfo::ObjectInfo(hid_t const id):

        _info{}

    {
        auto status = H5Oget_info(id, &_info);

        if (status < 0)
        {
            throw std::runtime_error("Cannot retrieve object info");
        }
    }


    auto ObjectInfo::fileno() const -> unsigned long  // NOLINT(google-runtime-int)
    {
        return _info.fileno;
    }


    auto ObjectInfo::addr() const -> haddr_t
    {
        return _info.addr;
    }


    /*!
        @brief      Return whether two instances are equal

        Two instances are considered equal if they are pointing to the same object in the HDF5 dataset.
    */
    auto ObjectInfo::operator==(ObjectInfo const& other) const -> bool
    {
        return fileno() == other.fileno() && addr() == other.addr();
    }


    /*!
        @brief      Return whether two instances are not equal
        @sa         operator==(ObjectInfo const&) const

        Two instances are considered equal if they are pointing to the same object in the HDF5 dataset.
    */
    auto ObjectInfo::operator!=(ObjectInfo const& other) const -> bool
    {
        return !(*this == other);
    }

}  // namespace lue::hdf5
