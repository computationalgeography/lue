#include "lue/hdf5/shape.hpp"
#include <functional>
#include <numeric>


namespace lue {
    namespace hdf5 {

        /*!
            \brief      Return the size of an array of shape @a shape and
                        filled with elements of size @a size_of_element

            The value returned is in the same units as @a size_of_element. By
            passing 1 for this argument, you obtain the number of elements in
            the array.
        */
        Shape::value_type size_of_shape(Shape const& shape, std::size_t const size_of_element)
        {
            return std::accumulate(shape.begin(), shape.end(), size_of_element, std::multiplies<>{});
        }

    }  // namespace hdf5
}  // namespace lue
