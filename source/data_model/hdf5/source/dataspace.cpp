#include "lue/hdf5/dataspace.hpp"
#include <cassert>
#include <cstring>
#include <stdexcept>


namespace lue::hdf5 {

    /*!
        @brief      Construct an instance of a particular @a type
        @param      type Dataspace type
        @exception  std::runtime_error In case the dataspace cannot be created
        @sa         [H5Screate](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5S.html#Dataspace-Create)
    */
    Dataspace::Dataspace(H5S_class_t const type):

        Dataspace{H5Screate(type)}

    {
    }


    /*!
        @brief      Create an instance based on a dataspace identifier
        @param      id Identifier of created dataspace
        @exception  std::runtime_error In case the @a id is not valid
    */
    Dataspace::Dataspace(hid_t const id):

        Dataspace{Identifier{id, H5Sclose}}

    {
    }


    /*!
        @brief      Create an instance based on a dataspace identifier
        @param      id Identifier of created dataspace
        @exception  std::runtime_error In case the @a id is not valid
    */
    Dataspace::Dataspace(Identifier&& id):

        _id{std::move(id)}

    {
        if (!_id.is_valid())
        {
            throw std::runtime_error("Dataspace is not valid");
        }

        assert(_id.type() == H5I_DATASPACE);
    }


    /*!
        @brief      Return the identifier
    */
    auto Dataspace::id() const -> Identifier const&
    {
        return _id;
    }


    /*!
        @brief      Return the number of dimensions
        @exception  std::runtime_error In case the number of dimensions cannot be determined
    */
    auto Dataspace::nr_dimensions() const -> int
    {
        int const nr_dimensions{H5Sget_simple_extent_ndims(_id)};

        if (nr_dimensions < 0)
        {
            throw std::runtime_error("Cannot determine number of dataspace dimensions");
        }

        return nr_dimensions;
    }


    /*!
        @brief      Return the dimension extents
        @exception  std::runtime_error In case the dimension extents cannot be determined
    */
    auto Dataspace::dimension_extents() const -> Shape
    {
        int const nr_dimensions{this->nr_dimensions()};

        static_assert(std::is_same_v<Shape::value_type, hsize_t>);

        Shape extents(nr_dimensions);
        hsize_t* max_extents = nullptr;

        int const nr_dimensions2{H5Sget_simple_extent_dims(_id, extents.data(), max_extents)};

        if (nr_dimensions2 < 0)
        {
            throw std::runtime_error("Cannot retrieve dataspace extents");
        }

        assert(nr_dimensions2 == nr_dimensions);

        return extents;
    }


    auto Dataspace::nr_elements() const -> hssize_t
    {
        return H5Sget_simple_extent_npoints(_id);
    }


    /*!
        @brief      Create a new simple dataspace instance
        @param      dimension_sizes Size of each dimension. This value will also be used as the upper limit
                    on the size of each dimension
        @sa         create_dataspace(Shape const&, Shape const&)
    */
    auto create_dataspace(Shape const& dimension_sizes) -> Dataspace
    {
        return create_dataspace(dimension_sizes, dimension_sizes);
    }


    /*!
        @brief      Create a new simple dataspace instance
        @param      dimension_sizes Size of each dimension
        @param      max_dimension_sizes Upper limit on the size of each dimension
        @exception  std::runtime_error In case the dataspace cannot be created
        @sa [H5Screate_simple](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5S.html#Dataspace-CreateSimple)
    */
    auto create_dataspace(Shape const& dimension_sizes, Shape const& max_dimension_sizes) -> Dataspace
    {
        assert(dimension_sizes.size() == max_dimension_sizes.size());

        Identifier dataspace_location(
            H5Screate_simple(
                static_cast<int>(dimension_sizes.size()), dimension_sizes.data(), max_dimension_sizes.data()),
            H5Sclose);

        if (!dataspace_location.is_valid())
        {
            throw std::runtime_error("Dataspace cannot be created");
        }

        return Dataspace{std::move(dataspace_location)};
    }

}  // namespace lue::hdf5
