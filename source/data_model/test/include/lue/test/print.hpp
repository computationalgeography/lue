#pragma once
#include "lue/core/define.hpp"
#include "lue/hdf5.hpp"
#include <ostream>
#include <iterator>
#include <string>
#include <vector>


namespace lue {
namespace data_model {
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
}  // namespace data_model


namespace hdf5 {

inline std::ostream& operator<<(
    std::ostream& stream,
    hdf5::Offset const& offset)
{
    stream << "[";
    data_model::test::print(stream, offset, ":");
    stream << "]";
    return stream;
}


inline std::ostream& operator<<(
    std::ostream& stream,
    hdf5::Stride const& stride)
{
    stream << "[";
    data_model::test::print(stream, stride, ":");
    stream << "]";
    return stream;
}


inline std::ostream& operator<<(
    std::ostream& stream,
    hdf5::Count const& count)
{
    stream << "[";
    data_model::test::print(stream, count, ":");
    stream << "]";
    return stream;
}


inline std::ostream& operator<<(
    std::ostream& stream,
    hdf5::Shape const& shape)
{
    stream << "[";
    data_model::test::print(stream, shape, ":");
    stream << "]";
    return stream;
}


inline std::ostream& operator<<(
    std::ostream& stream,
    hdf5::Hyperslab const& hyperslab)
{
    stream << "[";
    data_model::test::print(stream, hyperslab.start(), ":");
    stream << ",";
    data_model::test::print(stream, hyperslab.stride(), ":");
    stream << ",";
    data_model::test::print(stream, hyperslab.count(), ":");
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


namespace data_model {

template<
    typename T>
inline std::ostream& write(
    std::ostream& stream,
    Aspect<T> const& aspect)
{
    stream << aspect.value();

    return stream;
}


#define STREAM_ASPECT(T)               \
inline std::ostream& operator<<(       \
    std::ostream& stream,              \
    T const kind)                      \
{                                      \
    stream << aspect_to_string(kind);  \
                                       \
    return stream;                     \
}

STREAM_ASPECT(ShapePerObject)
STREAM_ASPECT(ValueVariability)
STREAM_ASPECT(ShapeVariability)
STREAM_ASPECT(Mobility)
STREAM_ASPECT(SpaceDomainItemType)
STREAM_ASPECT(TimeDiscretization)
STREAM_ASPECT(SpaceDiscretization)
STREAM_ASPECT(TimeDomainItemType)
STREAM_ASPECT(time::Unit)

#undef STREAM_ASPECT


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
    [[maybe_unused]] std::index_sequence<indices...> const& sequence)
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
    return write(stream, configuration, std::index_sequence_for<Ts...>());
}


inline std::ostream& operator<<(
    std::ostream& stream,
    Clock const& clock)
{
    stream << clock.nr_units() << "[" << clock.unit() << "]";

    return stream;
}

}  // namespace data_model
}  // namespace lue
