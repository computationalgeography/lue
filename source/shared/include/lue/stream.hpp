#pragma once
#include <algorithm>
#include <array>
#include <experimental/iterator>
#include <ostream>


// The output stream operators defined here also allow the use of compound
// types when formatting strings with the format library. In that case,
// include fmt/ostream.h as well.


namespace std {

template<
    typename Element,
    std::size_t size>
ostream& operator<<(
    ostream& stream,
    array<Element, size> const& array)
    // char const prefix='[',
    // char const suffix=']')
{
    // stream << prefix;

    std::copy(
        std::begin(array), std::end(array),
        std::experimental::make_ostream_joiner(stream, ", "));

    // stream << suffix;

    return stream;
}

}  // namespace std
