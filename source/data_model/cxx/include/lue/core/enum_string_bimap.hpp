#pragma once
#include <boost/predef.h>
#include <boost/version.hpp>

#if BOOST_COMP_MSVC
#if (_MSC_VER == 1930)
#if (BOOST_VERSION <= 107400)
#pragma warning(push)
// 4834: discarding return value of function with 'nodiscard' attribute
#pragma warning(disable : 4834)
#endif
#endif
#endif
#include <boost/bimap.hpp>
#if BOOST_COMP_MSVC
#if (_MSC_VER == 1930)
#if (BOOST_VERSION <= 107400)
#pragma warning(pop)
#endif
#endif
#endif

#include <cassert>
#include <string>


namespace lue {
    namespace data_model {
        namespace detail {

            template<typename T>
            class EnumStringBimap
            {

                public:

                    using Map = boost::bimap<T, std::string>;
                    using value_type = typename Map::value_type;

                    EnumStringBimap() = default;

                    explicit EnumStringBimap(std::initializer_list<value_type> const& values);

                    EnumStringBimap(EnumStringBimap const&) = default;

                    EnumStringBimap(EnumStringBimap&&) noexcept = default;

                    ~EnumStringBimap() = default;

                    EnumStringBimap& operator=(EnumStringBimap const&) = default;

                    EnumStringBimap& operator=(EnumStringBimap&&) noexcept = default;

                    bool contains(std::string const& string) const;

                    T as_value(std::string const& string) const;

                    std::string as_string(T value) const;

                private:

                    Map _map;
            };


            template<typename T>
            inline EnumStringBimap<T>::EnumStringBimap(std::initializer_list<value_type> const& values):

                _map(values.begin(), values.end())

            {
            }


            template<typename T>
            inline bool EnumStringBimap<T>::contains(std::string const& string) const
            {
                return _map.right.find(string) != _map.right.end();
            }


            template<typename T>
            inline T EnumStringBimap<T>::as_value(std::string const& string) const
            {
                assert(contains(string));

                return _map.right.at(string);
            }


            template<typename T>
            inline std::string EnumStringBimap<T>::as_string(T const value) const
            {
                assert(_map.left.find(value) != _map.left.end());

                return _map.left.at(value);
            }

        }  // namespace detail
    }  // namespace data_model
}  // namespace lue
