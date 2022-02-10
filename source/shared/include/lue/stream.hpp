#pragma once
#include <algorithm>
#include <array>
// TODO Refactor with similar blocks in other stream.hpp headers.
#include <boost/predef.h>
#if BOOST_COMP_MSVC
#   include <boost/io/ostream_joiner.hpp>
#   define lue_make_ostream_joiner boost::io::make_ostream_joiner
#else
#   include <experimental/iterator>
#   define lue_make_ostream_joiner std::experimental::make_ostream_joiner
#endif
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
        lue_make_ostream_joiner(stream, ", "));

    // stream << suffix;

    return stream;
}

}  // namespace std
