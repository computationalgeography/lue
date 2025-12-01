#pragma once
#include "lue/hdf5/datatype.hpp"
#include "lue/hdf5/hyperslab.hpp"
#include "lue/string.hpp"
#include <ostream>


namespace lue::hdf5 {

    inline auto operator<<(std::ostream& stream, Offset const& offset) -> std::ostream&
    {
        return stream << "[" << join(offset, ":") << "]";
    }


    inline auto operator<<(std::ostream& stream, Stride const& stride) -> std::ostream&
    {
        return stream << "[" << join(stride, ":") << "]";
    }


    inline auto operator<<(std::ostream& stream, Count const& count) -> std::ostream&
    {
        return stream << "[" << join(count, ":") << "]";
    }


    inline auto operator<<(std::ostream& stream, Shape const& shape) -> std::ostream&
    {
        return stream << "[" << join(shape, ":") << "]";
    }


    inline auto operator<<(std::ostream& stream, Hyperslab const& hyperslab) -> std::ostream&
    {
        return stream << "[" << join(hyperslab.start(), ":") << "," << join(hyperslab.stride(), ":") << ","
                      << join(hyperslab.count(), ":") << "]";
    }


    inline auto operator<<(std::ostream& stream, Datatype const& datatype) -> std::ostream&
    {
        if (datatype.is_native())
        {
            stream << native_datatype_as_string(datatype);
        }
        else if (datatype.is_standard())
        {
            stream << standard_datatype_as_string(datatype);
        }
        else if (datatype.is_string())
        {
            stream << "string";
        }
        else
        {
            stream << "user defined datatype";
        }

        return stream;
    }

}  // namespace lue::hdf5
