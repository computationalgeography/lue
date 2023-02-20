#pragma once
#include <hdf5.h>


namespace lue {
    namespace hdf5 {

        class ObjectInfo
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

    }  // namespace hdf5
}  // namespace lue
