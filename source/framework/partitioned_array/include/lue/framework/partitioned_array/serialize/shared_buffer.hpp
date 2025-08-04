#pragma once
#include "lue/framework/partitioned_array/shared_buffer.hpp"
#include <hpx/serialization/array.hpp>


namespace hpx::serialization {

    template<typename Element>
    void serialize(
        input_archive& archive, lue::SharedBuffer<Element>& buffer, unsigned int const /* version */)
    {
        // Read buffer from archive

        using Buffer = lue::SharedBuffer<Element>;
        using Size = typename Buffer::Size;

        // Read buffer size and make sure buffer has enough room for the elements
        Size size{};
        archive & size;
        buffer.resize(size);

        // Read elements
        archive& hpx::serialization::make_array(buffer.data(), size);
    }


    template<typename Element>
    void serialize(
        output_archive& archive, lue::SharedBuffer<Element> const& buffer, unsigned int const /* version */)
    {
        // Write buffer to archive

        archive & buffer.size() & hpx::serialization::make_array(buffer.data(), buffer.size());
    }

}  // namespace hpx::serialization
