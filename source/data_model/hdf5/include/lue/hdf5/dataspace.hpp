#pragma once
#include "lue/hdf5/identifier.hpp"
#include "lue/hdf5/shape.hpp"


namespace lue::hdf5 {

    /*!
        @brief      Class for representing the size and shape of a dataset or attribute raw data
        @sa         [H5S: Dataspace Interface](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5S.html)
    */
    class LUE_HDF5_EXPORT Dataspace
    {

        public:

            explicit Dataspace(H5S_class_t type);

            explicit Dataspace(hid_t id);

            explicit Dataspace(Identifier&& id);

            Dataspace(Dataspace const&) = default;

            Dataspace(Dataspace&&) = default;

            ~Dataspace() = default;

            auto operator=(Dataspace const&) -> Dataspace& = default;

            auto operator=(Dataspace&&) -> Dataspace& = default;

            auto id() const -> Identifier const&;

            auto nr_dimensions() const -> int;

            auto dimension_extents() const -> Shape;

            auto nr_elements() const -> hssize_t;

        private:

            //! Id of dataspace
            Identifier _id;
    };


    LUE_HDF5_EXPORT auto create_dataspace(Shape const& dimension_sizes) -> Dataspace;

    LUE_HDF5_EXPORT auto create_dataspace(Shape const& dimension_sizes, Shape const& max_dimension_sizes)
        -> Dataspace;

}  // namespace lue::hdf5
