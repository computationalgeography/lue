#pragma once
#include <hdf5.h>
#include <vector>


namespace lue::hdf5 {

    /*!
        @brief      Class for instances representing counts in each dimension
        @sa         Offset, Stride

        Using this class instead of HDF5's `hsize_t const*` is more type-safe.
    */
    class Count: public std::vector<hsize_t>
    {
            using std::vector<hsize_t>::vector;
    };

}  // namespace lue::hdf5
