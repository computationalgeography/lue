#pragma once
#include "lue/framework/core/shared_buffer.hpp"
#include <hpx/serialization/array.hpp>


namespace hpx {
namespace serialization {

template<
    typename Element>
void serialize(
    input_archive& archive,
    lue::SharedBuffer<Element>& buffer,
    unsigned int const /* version */)
{
    // Read buffer from archive
    using Buffer = lue::SharedBuffer<Element>;
    using Array = hpx::serialization::array<Element>;

    typename Buffer::Size size;
    archive & size;

    // Otherwise, update folowing logic
    assert(size > 0);

    buffer.resize(size);

    Array array = hpx::serialization::make_array(buffer.data(), size);
    archive & array;
}


template<
    typename Element>
void serialize(
    output_archive& archive,
    lue::SharedBuffer<Element> const& buffer,
    unsigned int const /* version */)
{
    // Write buffer to archive
    using Buffer = lue::SharedBuffer<Element>;
    using Array = hpx::serialization::array<Element const>;

    // Otherwise, update folowing logic
    assert(!buffer.empty());

    typename Buffer::Size const size = buffer.size();
    Array array = hpx::serialization::make_array(buffer.data(), size);

    archive & size & array;
}

}  // namespace serialization
}  // namespace hpx
