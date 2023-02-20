#pragma once
#include "lue/hdf5/dataspace.hpp"
#include "lue/hdf5/datatype.hpp"


namespace lue {
    namespace hdf5 {

        class VLenMemory
        {

            public:

                VLenMemory(Datatype const& datatype, Dataspace const& dataspace, void* buffer);

                VLenMemory(VLenMemory const&) = delete;

                VLenMemory(VLenMemory&&) = delete;

                ~VLenMemory();

                VLenMemory& operator=(VLenMemory const&) = delete;

                VLenMemory& operator=(VLenMemory&&) = delete;

            private:

                Datatype _datatype;

                Dataspace _dataspace;

                void* _buffer;
        };

    }  // namespace hdf5
}  // namespace lue
