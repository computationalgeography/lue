#pragma once
#include "lue/framework/core/shared_buffer.hpp"
#include <hpx/runtime/serialization/array.hpp>


namespace hpx {
namespace serialization {

template<
    typename Element>
void serialize(
    input_archive& archive,
    lue::SharedBuffer<Element>& buffer,
    unsigned int const /* version */)
{
    using Buffer = lue::SharedBuffer<Element>;
    using Array = hpx::serialization::array<Element>;

    typename Buffer::Size size;
    archive & size;

    buffer.resize(size);

    Array array = hpx::serialization::make_array(buffer.begin(), size);
    archive & array;
}


template<
    typename Element>
void serialize(
    output_archive& archive,
    lue::SharedBuffer<Element> const& buffer,
    unsigned int const /* version */)
{
    using Buffer = lue::SharedBuffer<Element>;
    using Array = hpx::serialization::array<Element const>;

    typename Buffer::Size const size = buffer.size();
    Array array = hpx::serialization::make_array(buffer.begin(), size);

    archive & size & array;
}

}  // namespace serialization
}  // namespace hpx
