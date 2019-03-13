#include "lue/framework/core/configuration_entry.hpp"
#include <fmt/format.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>


namespace lue {
namespace {

std::string application_section()
{
    static auto const program_name =
        boost::filesystem::path(
                hpx::get_config_entry("hpx.program_name", "")
            ).filename().string();
    static auto const result = fmt::format("application.{}", program_name);

    return result;
}


std::string configuration_entry(
    std::string const& section,
    std::string const& key,
    std::string const& default_value)
{
    return hpx::get_config_entry(
        fmt::format("{}.{}", section, key), default_value);
}


template<
    typename T>
T cast(
    std::string const& value)
{
    T result;

    try {
        result = boost::lexical_cast<T>(value);
    }
    catch(boost::bad_lexical_cast const&) {
        throw std::runtime_error(
            fmt::format(
                "Cannot cast value {} to type {}",
                value, Type<T>::name()));
    }

    return result;
}


template<>
std::string cast<std::string>(
    std::string const& value)
{
    return value;
}

}  // Anonymous namespace


template<
    typename T>
T required_configuration_entry(
    std::string const& section,
    std::string const& key)
{
    auto const value = configuration_entry(section, key, "");

    if(value.empty()) {
        throw std::runtime_error(
            fmt::format(
                "Required configuration entry {}.{} is missing",
                section, key));
    }

    return cast<T>(value);
}


template<
    typename T>
T required_configuration_entry(
    std::string const& key)
{
    return required_configuration_entry<T>(application_section(), key);
}


template<
    typename T>
T optional_configuration_entry(
    std::string const& section,
    std::string const& key,
    T const& default_value)
{
    auto const value = configuration_entry(section, key, "");

    return !value.empty() ? cast<T>(value) : default_value;
}


template<
    typename T>
T optional_configuration_entry(
    std::string const& key,
    T const& default_value)
{
    return optional_configuration_entry<T>(
        application_section(), key, default_value);
}


#define REQUIRED_CONFIGURATION_ENTRY(              \
    Type)                                          \
template Type required_configuration_entry<Type>(  \
    std::string const& section,                    \
    std::string const& key);                       \
                                                   \
template Type required_configuration_entry<Type>(  \
    std::string const& key);


#define OPTIONAL_CONFIGURATION_ENTRY(                      \
    Type)                                                  \
template Type optional_configuration_entry<Type>(          \
    std::string const&, std::string const&, Type const&);  \
                                                           \
template Type optional_configuration_entry<Type>(          \
    std::string const&, Type const&);

REQUIRED_CONFIGURATION_ENTRY(std::uint32_t);
REQUIRED_CONFIGURATION_ENTRY(std::uint64_t);
REQUIRED_CONFIGURATION_ENTRY(std::string);

OPTIONAL_CONFIGURATION_ENTRY(std::uint32_t);
OPTIONAL_CONFIGURATION_ENTRY(std::uint64_t);
OPTIONAL_CONFIGURATION_ENTRY(std::string);

#undef REQUIRED_CONFIGURATION_ENTRY
#undef OPTIONAL_CONFIGURATION_ENTRY

}  // namespace lue
