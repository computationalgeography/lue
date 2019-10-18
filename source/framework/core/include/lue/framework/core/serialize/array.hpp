#pragma once
#include "lue/framework/core/array.hpp"
#include <hpx/runtime/serialization/array.hpp>


namespace hpx {
namespace serialization {

template<
    typename Element,
    std::size_t rank>
void serialize(
    input_archive& archive,
    lue::Array<Element, rank>& array,
    unsigned int const /* version */)
{
    // using Buffer = lue::SharedBuffer<Element>;
    // using Array = hpx::serialization::array<Element>;

    // typename Buffer::Count size;
    // archive & size;

    // buffer.resize(size);

    // Array array = hpx::serialization::make_array(buffer.begin(), size);
    // archive & array;
}


template<
    typename Element,
    std::size_t rank>
void serialize(
    output_archive& archive,
    lue::Array<Element, rank> const& array,
    unsigned int const /* version */)
{
    // using Buffer = lue::SharedBuffer<Element>;
    // using Array = hpx::serialization::array<Element const>;

    // typename Buffer::Count const size = buffer.size();
    // Array array = hpx::serialization::make_array(buffer.begin(), size);

    // archive & size & array;
}

}  // namespace serialization
}  // namespace hpx
