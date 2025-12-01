#pragma once
#include "lue/hdf5/count.hpp"
#include "lue/hdf5/offset.hpp"
#include "lue/hdf5/shape.hpp"
#include "lue/hdf5/stride.hpp"


namespace lue::hdf5 {

    /*!
        @brief      Class representing a hyperslab selection of a dataspace
        @sa         [Reading from or writing to a subset of a dataset](
                    https://support.hdfgroup.org/HDF5/Tutor/selectsimple.html)

        A hyperslab defines a subset of a dataspace.
    */
    class LUE_HDF5_EXPORT Hyperslab
    {

        public:

            Hyperslab() = default;

            Hyperslab(Offset start, Stride stride, Count count);

            Hyperslab(Offset start, Count count);

            explicit Hyperslab(Shape const& shape);

            Hyperslab(Hyperslab const& other) = default;

            Hyperslab(Hyperslab&& other) = default;

            ~Hyperslab() = default;

            auto operator=(Hyperslab const& other) -> Hyperslab& = default;

            auto operator=(Hyperslab&& other) -> Hyperslab& = default;

            auto start() const -> Offset const&;

            auto start() -> Offset&;

            auto stride() const -> Stride const&;

            auto stride() -> Stride&;

            auto count() const -> Count const&;

            auto count() -> Count&;

            auto nr_dimensions() const -> std::size_t;

            auto empty() const -> bool;

            auto nr_elements() const -> std::size_t;

        private:

            //! Offset of the starting element
            Offset _start;

            //! Amount of elements to move in each direction
            Stride _stride;

            //! Number of blocks to select
            Count _count;
    };


    LUE_HDF5_EXPORT auto hyperslab(
        Offset const& subset_center, Shape const& subset_shape, Shape const& array_shape) -> Hyperslab;

}  // namespace lue::hdf5
