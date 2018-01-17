#pragma once
#include <ostream>
#include <string>
#include <vector>
#include "lue/hdf5.hpp"


namespace lue {
namespace test {

template<
    typename T>
inline void print(
    std::ostream& stream,
    std::vector<T> const& values,
    std::string const& separator)
{
    if(!values.empty()) {
        std::copy(
            values.begin(), --values.end(),
            std::ostream_iterator<T>(stream, separator.c_str()));
        stream << values.back();
    }
}

}  // namespace test


namespace hdf5 {

inline std::ostream& operator<<(
    std::ostream& stream,
    hdf5::Offset const& offset)
{
    stream << "[";
    test::print(stream, offset, ":");
    stream << "]";
    return stream;
}


inline std::ostream& operator<<(
    std::ostream& stream,
    hdf5::Stride const& stride)
{
    stream << "[";
    test::print(stream, stride, ":");
    stream << "]";
    return stream;
}


inline std::ostream& operator<<(
    std::ostream& stream,
    hdf5::Count const& count)
{
    stream << "[";
    test::print(stream, count, ":");
    stream << "]";
    return stream;
}


inline std::ostream& operator<<(
    std::ostream& stream,
    hdf5::Shape const& shape)
{
    stream << "[";
    test::print(stream, shape, ":");
    stream << "]";
    return stream;
}


inline std::ostream& operator<<(
    std::ostream& stream,
    hdf5::Hyperslab const& hyperslab)
{
    stream << "[";
    test::print(stream, hyperslab.start(), ":");
    stream << ",";
    test::print(stream, hyperslab.stride(), ":");
    stream << ",";
    test::print(stream, hyperslab.count(), ":");
    stream << "]";

    return stream;
}

}  // namespace hdf5
}  // namespace lue
