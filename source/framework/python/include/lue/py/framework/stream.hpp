#pragma once
#include "lue/py/framework/type_traits.hpp"
#include "lue/framework/core/shape.hpp"
#include <experimental/iterator>
#include <sstream>
#include <tuple>


namespace lue::framework {

    template<
        typename OutputStream,
        typename... Type>
    OutputStream& operator<<(
        OutputStream& stream,
        std::tuple<Type...> const& tuple)
    {
        stream << '(';
        std::apply(

                [&stream](
                    auto&&... ts)
                {
                    ((stream << ts << ", "), ...);
                },

            tuple);
        stream << ')';

        return stream;
    }


    template<
        typename OutputStream,
        typename Count,
        Rank rank>
    OutputStream& operator<<(
        OutputStream& stream,
        Shape<Count, rank> const& shape)
    {
        stream << '(';
        std::copy(
            std::begin(shape), std::end(shape),
            std::experimental::make_ostream_joiner(stream, ", "));
        stream << ')';

        return stream;
    }


    template<
        typename Count,
        Rank rank>
    std::string as_string(
        Shape<Count, rank> const& shape)
    {
        std::ostringstream stream;
        stream << shape;
        return stream.str();
    }

}  // lue::framework
