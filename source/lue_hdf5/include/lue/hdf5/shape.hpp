#pragma once
#include <hdf5.h>
#include <vector>


namespace lue {
namespace hdf5 {

class Shape: public std::vector<hsize_t> {
    using std::vector<hsize_t>::vector;
};


Shape::value_type  size_of_shape       (Shape const& shape,
                                        std::size_t const size_of_element);

}  // namespace hdf5
}  // namespace lue
