#pragma once
#include "lue/hdf5/export.hpp"
#include <hdf5.h>


namespace lue::hdf5 {

    class LUE_HDF5_EXPORT ObjectInfo
    {

        public:

            explicit ObjectInfo(hid_t id);

            ObjectInfo(ObjectInfo const& other) = default;

            ObjectInfo(ObjectInfo&& other) = default;

            ~ObjectInfo() = default;

            auto operator=(ObjectInfo const& other) -> ObjectInfo& = default;

            auto operator=(ObjectInfo&& other) -> ObjectInfo& = default;

            auto operator==(ObjectInfo const& other) const -> bool;

            auto operator!=(ObjectInfo const& other) const -> bool;

            // NOLINTNEXTLINE(google-runtime-int)
            auto fileno() const -> unsigned long;

            auto addr() const -> haddr_t;

        private:

            H5O_info_t _info;
    };

}  // namespace lue::hdf5
