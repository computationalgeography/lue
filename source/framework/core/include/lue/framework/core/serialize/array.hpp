#pragma once
#include "lue/framework/core/array.hpp"
#include <hpx/runtime/serialization/array.hpp>


namespace hpx {
namespace serialization {

template<
    typename Element,
    lue::Rank rank>
void serialize(
    input_archive& archive,
    lue::Array<Element, rank>& array,
    unsigned int const /* version */)
{
    using Array = lue::Array<Element, rank>;

    // Read array shape and make sure array has enough room for the elements
    typename Array::Shape shape;
    archive & shape;
    array.reshape(shape);

    // Read elements
    hpx::serialization::array<Element> elements =
        hpx::serialization::make_array(array.data(), lue::nr_elements(array));
    archive & elements;
}


template<
    typename Element,
    lue::Rank rank>
void serialize(
    output_archive& archive,
    lue::Array<Element, rank> const& array,
    unsigned int const /* version */)
{
    archive
        & array.shape()
        & hpx::serialization::make_array(array.data(), lue::nr_elements(array))
        ;
}

}  // namespace serialization
}  // namespace hpx
