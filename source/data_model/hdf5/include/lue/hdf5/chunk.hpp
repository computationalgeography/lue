#pragma once
#include "lue/hdf5/shape.hpp"


namespace lue {
namespace hdf5 {

std::size_t        lower_chunk_size_limit();

std::size_t        upper_chunk_size_limit();

Shape              chunk_shape         (Shape const& value_shape,
                                        std::size_t size_of_element);

Shape::value_type  size_of_chunk       (Shape const& chunk,
                                        std::size_t size_of_element);


template<
    typename T>
inline Shape chunk_shape(
    Shape const& value_shape)
{
    return chunk_shape(value_shape, sizeof(T));
}


template<
    typename T>
inline Shape::value_type size_of_chunk(
    Shape const& value_shape)
{
    return size_of_chunk(value_shape, sizeof(T));
}

}  // namespace hdf5
}  // namespace lue
