#pragma once
#include <hpx/runtime/config_entry.hpp>


namespace lue {

template<
    typename T>
struct Type
{
    static char const* name();
};


#define TYPE(T)                \
template<>                     \
struct Type<T>                 \
{                              \
    static char const* name()  \
    {                          \
        return #T;             \
    }                          \
};

TYPE(std::uint64_t)
#undef TYPE


template<
    typename T>
T                  required_configuration_entry(
                                        std::string const& section,
                                        std::string const& key);

template<
    typename T>
T                  required_configuration_entry(
                                        std::string const& key);

template<
    typename T>
T                  optional_configuration_entry(
                                        std::string const& section,
                                        std::string const& key,
                                        T const& default_value);

template<
    typename T>
T                  optional_configuration_entry(
                                        std::string const& key,
                                        T const& default_value);

}  // namespace lue
