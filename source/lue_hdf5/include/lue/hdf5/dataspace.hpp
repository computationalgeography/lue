#pragma once
#include "lue/hdf5/identifier.hpp"
#include "lue/hdf5/shape.hpp"


namespace lue {
namespace hdf5 {

/*!
    @brief      Class for representing the size and shape of a dataset or
                attribute raw data
    @sa         [H5S: Dataspace Interface](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5S.html)
*/
class Dataspace
{

public:

    explicit       Dataspace           (::H5S_class_t type);

    explicit       Dataspace           (hid_t id);

    explicit       Dataspace           (Identifier&& id);

                   Dataspace           (Dataspace const&)=default;

                   Dataspace           (Dataspace&&)=default;

                   ~Dataspace          ()=default;

    Dataspace&     operator=           (Dataspace const&)=default;

    Dataspace&     operator=           (Dataspace&&)=default;

    Identifier const& id               () const;

    int            nr_dimensions       () const;

    Shape          dimension_extents   () const;

    hssize_t       nr_elements         () const;

private:

    //! Id of dataspace
    Identifier     _id;

};


Dataspace          create_dataspace    (Shape const& dimension_sizes);

Dataspace          create_dataspace    (Shape const& dimension_sizes,
                                        Shape const& max_dimension_sizes);

} // namespace hdf5
} // namespace lue
