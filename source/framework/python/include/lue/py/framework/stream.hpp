#pragma once
#include "lue/framework/core/shape.hpp"
#include "lue/py/framework/type_traits.hpp"
// TODO Refactor with similar blocks in other stream.hpp headers.
#include <boost/predef.h>
#if BOOST_COMP_MSVC
#include <boost/io/ostream_joiner.hpp>
#define lue_make_ostream_joiner boost::io::make_ostream_joiner
#else
#include <experimental/iterator>
#define lue_make_ostream_joiner std::experimental::make_ostream_joiner
#endif
#include <sstream>
#include <tuple>


namespace lue::framework {

    template<typename OutputStream, typename... Type>
    OutputStream& operator<<(OutputStream& stream, std::tuple<Type...> const& tuple)
    {
        stream << '(';
        std::apply(

            [&stream](auto&&... ts) { ((stream << ts << ", "), ...); },

            tuple);
        stream << ')';

        return stream;
    }


    template<typename OutputStream, typename Count, Rank rank>
    OutputStream& operator<<(OutputStream& stream, Shape<Count, rank> const& shape)
    {
        stream << '(';
        std::copy(std::begin(shape), std::end(shape), lue_make_ostream_joiner(stream, ", "));
        stream << ')';

        return stream;
    }


    template<typename Count, Rank rank>
    std::string as_string(Shape<Count, rank> const& shape)
    {
        std::ostringstream stream;
        stream << shape;
        return stream.str();
    }

}  // namespace lue::framework
