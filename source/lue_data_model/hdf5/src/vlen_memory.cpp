#include "lue/hdf5/vlen_memory.hpp"
#include <cassert>


namespace lue {
namespace hdf5 {

VLenMemory::VLenMemory(
    Datatype const& datatype,
    Dataspace const& dataspace,
    void* buffer):

    _datatype{datatype},
    _dataspace{dataspace},
    _buffer{buffer}

{
    assert(_buffer != nullptr);
}


VLenMemory::~VLenMemory()
{
    assert(_buffer != nullptr);

    ::H5Dvlen_reclaim(_datatype.id(), _dataspace.id(), H5P_DEFAULT, _buffer);
}

} // namespace hdf5
} // namespace lue
