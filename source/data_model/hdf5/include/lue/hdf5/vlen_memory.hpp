#pragma once
#include "lue/hdf5/dataspace.hpp"
#include "lue/hdf5/datatype.hpp"


namespace lue::hdf5 {

    class LUE_HDF5_EXPORT VLenMemory
    {

        public:

            VLenMemory(Datatype datatype, Dataspace dataspace, void* buffer);

            VLenMemory(VLenMemory const& other) = delete;

            VLenMemory(VLenMemory&& other) = delete;

            ~VLenMemory();

            auto operator=(VLenMemory const& other) -> VLenMemory& = delete;

            auto operator=(VLenMemory&& other) -> VLenMemory& = delete;

        private:

            Datatype _datatype;

            Dataspace _dataspace;

            void* _buffer;
    };

}  // namespace lue::hdf5
