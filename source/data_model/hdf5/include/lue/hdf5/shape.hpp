#pragma once
#include <hdf5.h>
#include <vector>


namespace lue {
namespace hdf5 {

/*!
    @brief      Class for instances representing shapes in each dimension

    Using this class instead of HDF5's `hsize_t const*` is more type-safe.
*/
class Shape: public std::vector<hsize_t> {
    using std::vector<hsize_t>::vector;
};


Shape::value_type  size_of_shape       (Shape const& shape,
                                        std::size_t size_of_element=1);

}  // namespace hdf5
}  // namespace lue
