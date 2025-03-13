#pragma once
#include "lue/hdf5/export.hpp"
#include <hdf5.h>


namespace lue::hdf5 {

    class LUE_HDF5_EXPORT ObjectInfo
    {

        public:

            explicit ObjectInfo(hid_t id);

            ObjectInfo(ObjectInfo const&) = default;

            ObjectInfo(ObjectInfo&&) = default;

            ~ObjectInfo() = default;

            ObjectInfo& operator=(ObjectInfo const&) = default;

            ObjectInfo& operator=(ObjectInfo&&) = default;

            bool operator==(ObjectInfo const& other) const;

            bool operator!=(ObjectInfo const& other) const;

            // NOLINTNEXTLINE(google-runtime-int)
            unsigned long fileno() const;

            haddr_t addr() const;

        private:

            H5O_info_t _info;
    };

}  // namespace lue::hdf5
