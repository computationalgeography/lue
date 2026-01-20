#pragma once
#include "lue/hdf5/export.hpp"
#include "lue/hdf5/shape.hpp"


namespace lue::hdf5 {

    auto lower_chunk_size_limit() -> std::size_t;

    auto upper_chunk_size_limit() -> std::size_t;

    LUE_HDF5_EXPORT auto chunk_shape(Shape const& value_shape, std::size_t size_of_element) -> Shape;

    auto size_of_chunk(Shape const& chunk, std::size_t size_of_element) -> Shape::value_type;


    template<typename T>
    auto chunk_shape(Shape const& value_shape) -> Shape
    {
        return chunk_shape(value_shape, sizeof(T));
    }


    template<typename T>
    auto size_of_chunk(Shape const& value_shape) -> Shape::value_type
    {
        return size_of_chunk(value_shape, sizeof(T));
    }

}  // namespace lue::hdf5
