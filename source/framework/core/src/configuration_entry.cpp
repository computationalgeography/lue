#include "lue/framework/core/configuration_entry.hpp"
#include "lue/framework/core/shape.hpp"
#include <fmt/format.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <regex>


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


template<>
std::vector<std::uint64_t> cast<std::vector<std::uint64_t>>(
    std::string const& value)
{
    // parse value into a vector of unsigned integers
    std::string const pattern{
        "\\[([[:digit:]]+)(?:,[[:space:]]*([[:digit:]]+))*\\]"};
    std::regex expression{pattern};
    std::smatch match;

    std::vector<std::uint64_t> result;

    if(std::regex_match(value, match, expression)) {
        result.reserve(match.size());

        for(std::size_t i = 1; i < match.size(); ++i) {
            result.push_back(cast<std::uint64_t>(match[i]));
        }
    }
    else {
        throw std::runtime_error(
            fmt::format(
                "Configuration entry {} cannot be parsed into "
                "a vector of unsigned integers (using pattern {})",
                value, pattern));
    }

    return result;
}


template<>
lue::Shape<std::uint64_t, 2> cast<lue::Shape<std::uint64_t, 2>>(
    std::string const& value)
{
    std::vector<std::uint64_t> values = cast<std::vector<std::uint64_t>>(value);

    if(values.size() != 2) {
        throw std::runtime_error(
            fmt::format(
                "Configuration entry {} must contain 2 values, "
                "but {} where found",
                value, values.size()));
    }

    lue::Shape<std::uint64_t, 2> result;
    std::copy(values.begin(), values.end(), result.begin());

    return result;
}


}  // Anonymous namespace


namespace detail {

// Using this template to get at the type passed into a macro, allows
// passing instantiated templates with multiple arguments to be passed.
// See below for use.
template<
    typename T>
struct ArgumentType
{
};


// Passing an template type instantiated with multiple arguments requires
// the use of parenthesis. This template is a trick to get at the type in
// the parenthesis, for use in the macro.
// In macro, use ArgumentType<void(T)>::Type to get at the actual type
// passed in, without the parenthesis.
template<
    typename T,
    typename U>
struct ArgumentType<T(U)>
{
   using Type = U;
};

}  // namespace detail


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


#define REQUIRED_CONFIGURATION_ENTRY(                                       \
    T)                                                                      \
template detail::ArgumentType<void(T)>::Type                                \
        required_configuration_entry<detail::ArgumentType<void(T)>::Type>(  \
    std::string const& section,                                             \
    std::string const& key);                                                \
                                                                            \
template detail::ArgumentType<void(T)>::Type                                \
        required_configuration_entry<detail::ArgumentType<void(T)>::Type>(  \
    std::string const& key);


#define OPTIONAL_CONFIGURATION_ENTRY(                                       \
    T)                                                                      \
template detail::ArgumentType<void(T)>::Type                                \
        optional_configuration_entry<detail::ArgumentType<void(T)>::Type>(  \
    std::string const&, std::string const&,                                 \
    detail::ArgumentType<void(T)>::Type const&);                            \
                                                                            \
template detail::ArgumentType<void(T)>::Type                                \
        optional_configuration_entry<detail::ArgumentType<void(T)>::Type>(  \
    std::string const&, detail::ArgumentType<void(T)>::Type const&);

REQUIRED_CONFIGURATION_ENTRY(std::uint32_t);
REQUIRED_CONFIGURATION_ENTRY(std::uint64_t);
REQUIRED_CONFIGURATION_ENTRY(std::string);
REQUIRED_CONFIGURATION_ENTRY(std::vector<std::uint64_t>);
REQUIRED_CONFIGURATION_ENTRY((lue::Shape<std::uint64_t, 2>));

OPTIONAL_CONFIGURATION_ENTRY(std::uint32_t);
OPTIONAL_CONFIGURATION_ENTRY(std::uint64_t);
OPTIONAL_CONFIGURATION_ENTRY(std::string);
OPTIONAL_CONFIGURATION_ENTRY(std::vector<std::uint64_t>);
OPTIONAL_CONFIGURATION_ENTRY((lue::Shape<std::uint64_t, 2>));

#undef REQUIRED_CONFIGURATION_ENTRY
#undef OPTIONAL_CONFIGURATION_ENTRY

}  // namespace lue
