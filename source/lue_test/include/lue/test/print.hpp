#pragma once
#include "lue/core/define.hpp"
#include "lue/hdf5.hpp"
#include <ostream>
#include <iterator>
#include <string>
#include <vector>


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


inline std::ostream& operator<<(
    std::ostream& stream,
    Datatype const& datatype)
{
    if(datatype.is_native()) {
        stream << native_datatype_as_string(datatype);
    }
    else if(datatype.is_standard()) {
        stream << standard_datatype_as_string(datatype);
    }
    else {
        stream << "user defined datatype";
    }

    return stream;
}

}  // namespace hdf5


inline std::ostream& operator<<(
    std::ostream& stream,
    ShapePerObject const kind)
{
    stream << kind;

    return stream;
}


inline std::ostream& operator<<(
    std::ostream& stream,
    ValueVariability const kind)
{
    stream << kind;

    return stream;
}


inline std::ostream& operator<<(
    std::ostream& stream,
    Mobility const kind)
{
    stream << kind;

    return stream;
}


inline std::ostream& operator<<(
    std::ostream& stream,
    SpaceDomainItemType const kind)
{
    stream << kind;

    return stream;
}


inline std::ostream& operator<<(
    std::ostream& stream,
    SpaceDiscretization const kind)
{
    stream << kind;

    return stream;
}


template<
    typename Aspect>
inline std::ostream& write(
    std::ostream& stream,
    Aspect const& aspect)
{
    stream << aspect.value();

    return stream;
}


template<
    typename Aspect,
    typename... Aspects>
inline std::ostream& write(
    std::ostream& stream,
    Aspect const& aspect,
    Aspects const&... aspects)
{
    write(stream, aspect);
    stream << ", ";
    write(stream, aspects...);

    return stream;
}


template<
    typename... Ts,
    std::size_t... indices>
inline std::ostream& write(
    std::ostream& stream,
    Configuration<Ts...> const& configuration,
    std::index_sequence<indices...> const&)
{
    write(stream, std::get<indices>(configuration.aspects())...);

    return stream;
}


template<
    typename... Ts>
inline std::ostream& operator<<(
    std::ostream& stream,
    Configuration<Ts...> const& configuration)
{
    return write(
        stream, configuration, std::index_sequence_for<Ts...>());
}

}  // namespace lue
