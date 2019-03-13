#pragma once
#include "lue/hdf5/dataspace.hpp"
#include "lue/hdf5/datatype.hpp"


namespace lue {
namespace hdf5 {

class VLenMemory
{

public:

                   VLenMemory          (Datatype const& datatype,
                                        Dataspace const& dataspace,
                                        void* buffer);

                   ~VLenMemory         ();

private:

    Datatype       _datatype;

    Dataspace      _dataspace;

    void*          _buffer;

};

} // namespace hdf5
} // namespace lue
