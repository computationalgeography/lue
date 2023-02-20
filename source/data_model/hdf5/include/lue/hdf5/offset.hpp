#pragma once
#include <hdf5.h>
#include <vector>


namespace lue {
    namespace hdf5 {

        /*!
            @brief      Class for instances representing offsets in each dimension
            @sa         Stride, Count

            Using this class instead of HDF5's `hsize_t const*` is more type-safe.
        */
        class Offset: public std::vector<::hsize_t>
        {
                using std::vector<::hsize_t>::vector;
        };

    }  // namespace hdf5
}  // namespace lue
