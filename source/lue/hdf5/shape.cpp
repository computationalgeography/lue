#include "lue/hdf5/shape.h"
#include <functional>
#include <numeric>


namespace lue {
namespace hdf5 {

Shape::value_type size_of_shape(
    Shape const& shape,
    std::size_t const size_of_element)
{
    return std::accumulate(
        shape.begin(), shape.end(),
        size_of_element,
        std::multiplies<Shape::value_type>());
}

}  // namespace hdf5
}  // namespace lue
