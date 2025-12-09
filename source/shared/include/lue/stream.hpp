#pragma once
#include <algorithm>
#include <array>
// TODO Refactor with similar blocks in other stream.hpp headers.
#include <boost/predef.h>
#if BOOST_COMP_MSVC
#include <boost/io/ostream_joiner.hpp>
#define lue_make_ostream_joiner boost::io::make_ostream_joiner
#else
#include <experimental/iterator>
#define lue_make_ostream_joiner std::experimental::make_ostream_joiner
#endif
#include <map>
#include <optional>
#include <ostream>
#include <vector>


// The output stream operators defined here also allow the use of compound
// types when formatting strings with the format library. In that case,
// include fmt/ostream.h as well.


namespace std {

    template<typename Element, std::size_t size>
    auto operator<<(ostream& stream, array<Element, size> const& array) -> ostream&
    {
        stream << '[';
        std::copy(std::begin(array), std::end(array), lue_make_ostream_joiner(stream, ", "));
        // Prints a separator after the last element...
        stream << ']';

        return stream;
    }


    template<typename T>
    auto operator<<(ostream& stream, optional<T> const& instance) -> ostream&
    {
        stream << '(';
        if (instance)
        {
            stream << *instance;
        }
        stream << ')';

        return stream;
    }


    template<typename... Ts>
    auto operator<<(std::ostream& stream, std::tuple<Ts...> const& tuple) -> std::ostream&
    {
        stream << '(';
        std::apply([&stream](auto&&... element) -> auto { ((stream << element << ", "), ...); }, tuple);
        stream << ')';

        return stream;
    }


    template<typename T>
    auto operator<<(ostream& stream, std::vector<T> const& vector) -> ostream&
    {
        stream << '[';

        {
            auto joiner = lue_make_ostream_joiner(stream, ", ");

            auto const nr_elements = vector.size();
            auto const begin = vector.begin();
            auto const end = vector.end();

            // Max number of values to print at start and end
            std::size_t const halo = 15;

            if (nr_elements <= 2 * halo)
            {
                // Print all values
                std::copy(begin, end, joiner);
            }
            else
            {
                // Print first and last halo number of values
                std::copy(begin, begin + halo, joiner);
                stream << ", ..., ";
                std::copy(end - halo, end, joiner);
            }
        }

        stream << "]";

        return stream;
    }


    template<typename Key, typename Value>
    auto operator<<(ostream& stream, std::map<Key, Value> const& map_) -> ostream&
    {
        stream << '{';

        {
            for (auto const& pair : map_)
            {
                stream << '<' << pair.first << ": " << pair.second << ">, ";
            }
        }

        stream << "}";

        return stream;
    }

}  // namespace std
