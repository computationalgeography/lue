#include "lue/hdf5/vlen_memory.hpp"
#include <cassert>
#include <utility>


namespace lue::hdf5 {

    VLenMemory::VLenMemory(Datatype datatype, Dataspace dataspace, void* buffer):

        _datatype{std::move(datatype)},
        _dataspace{std::move(dataspace)},
        _buffer{buffer}

    {
        assert(_buffer != nullptr);
    }


    VLenMemory::~VLenMemory()
    {
        assert(_buffer != nullptr);

        ::H5Dvlen_reclaim(_datatype.id(), _dataspace.id(), H5P_DEFAULT, _buffer);
    }

}  // namespace lue::hdf5
