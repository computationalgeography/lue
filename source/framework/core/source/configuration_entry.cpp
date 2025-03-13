#include "lue/framework/core/configuration_entry.hpp"
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/shape.hpp"
#include "lue/macro.hpp"  // ArgumentType
#include <hpx/modules/runtime_local.hpp>
#include <boost/lexical_cast.hpp>
#include <filesystem>
#include <format>
#include <regex>


namespace lue {
    namespace {

        std::string application_section()
        {
            static auto const program_name =
                std::filesystem::path(hpx::get_config_entry("hpx.program_name", "")).filename().string();
            static auto const result = std::format("application.{}", program_name);

            return result;
        }


        std::string configuration_entry(
            std::string const& section, std::string const& key, std::string const& default_value)
        {
            return hpx::get_config_entry(std::format("{}.{}", section, key), default_value);
        }


        std::string trim_vector(std::string const& value)
        {
            std::smatch match;

            // Trim string with list of values from the list delimiters
            if (!std::regex_match(value, match, std::regex(R"(\[([^\]]*)\])")))
            {
                throw std::runtime_error(std::format("List of values not delimited by '[]' ({})", value));
            }

            lue_hpx_assert(match.size() == 2);

            return match[1];
        }

    }  // Anonymous namespace


    namespace detail {
        namespace {

            template<typename T>
            std::vector<T> cast_list(std::string const& value, std::regex const& expression)
            {
                std::string string{trim_vector(value)};
                std::smatch match;
                std::vector<T> result;

                while (std::regex_search(string, match, expression))
                {
                    result.push_back(cast<T>(match[1]));
                    string = match.suffix().str();
                }

                if (!string.empty())
                {
                    throw std::runtime_error(std::format(
                        "Configuration entry '{}' cannot be parsed into "
                        "a list of values of type {}",
                        value,
                        Type<T>::name()));
                }

                return result;
            }

        }  // Anonymous namespace


        template<typename T>
        T cast(std::string const& value)
        {
            T result;

            try
            {
                result = boost::lexical_cast<T>(value);
            }
            catch (boost::bad_lexical_cast const&)
            {
                throw std::runtime_error(
                    std::format("Cannot cast value '{}' to type {}", value, Type<T>::name()));
            }

            return result;
        }


        template<>
        std::string cast<std::string>(std::string const& value)
        {
            return value;
        }


        template<>
        std::vector<std::uint64_t> cast<std::vector<std::uint64_t>>(std::string const& value)
        {
            return cast_list<std::uint64_t>(
                value, std::regex(R"([[:space:]]*([[:digit:]]+)[[:space:]]*(?:$|,))"));
        }


        template<>
        std::vector<std::int64_t> cast<std::vector<std::int64_t>>(std::string const& value)
        {
            // FIXME: Doesn't support negative integers yet
            return cast_list<std::int64_t>(
                value, std::regex(R"([[:space:]]*([[:digit:]]+)[[:space:]]*(?:$|,))"));
        }


        template<>
        lue::Shape<std::uint64_t, 2> cast<lue::Shape<std::uint64_t, 2>>(std::string const& value)
        {
            std::vector<std::uint64_t> values = cast<std::vector<std::uint64_t>>(value);

            if (values.size() != 2)
            {
                throw std::runtime_error(std::format(
                    "Configuration entry '{}' must contain 2 values, "
                    "but {} where found",
                    value,
                    values.size()));
            }

            lue::Shape<std::uint64_t, 2> result{};
            std::copy(values.begin(), values.end(), result.begin());

            return result;
        }


        template<>
        lue::Shape<std::int64_t, 2> cast<lue::Shape<std::int64_t, 2>>(std::string const& value)
        {
            std::vector<std::int64_t> values = cast<std::vector<std::int64_t>>(value);

            if (values.size() != 2)
            {
                throw std::runtime_error(std::format(
                    "Configuration entry '{}' must contain 2 values, "
                    "but {} where found",
                    value,
                    values.size()));
            }

            lue::Shape<std::int64_t, 2> result{};
            std::copy(values.begin(), values.end(), result.begin());

            return result;
        }

    }  // namespace detail


    template<typename T>
    T required_configuration_entry(std::string const& section, std::string const& key)
    {
        auto const value = configuration_entry(section, key, "");

        if (value.empty())
        {
            throw std::runtime_error(
                std::format("Required configuration entry {}.{} is missing", section, key));
        }

        return detail::cast<T>(value);
    }


    template<typename T>
    T required_configuration_entry(std::string const& key)
    {
        return required_configuration_entry<T>(application_section(), key);
    }


    template<typename T>
    T optional_configuration_entry(std::string const& section, std::string const& key, T const& default_value)
    {
        auto const value = configuration_entry(section, key, "");

        return !value.empty() ? detail::cast<T>(value) : default_value;
    }


    template<typename T>
    T optional_configuration_entry(std::string const& key, T const& default_value)
    {
        return optional_configuration_entry<T>(application_section(), key, default_value);
    }


#define REQUIRED_CONFIGURATION_ENTRY(T)                                                                      \
    template ArgumentType<void(T)> required_configuration_entry<ArgumentType<void(T)>>(                      \
        std::string const& section, std::string const& key);                                                 \
                                                                                                             \
    template ArgumentType<void(T)> required_configuration_entry<ArgumentType<void(T)>>(                      \
        std::string const& key);


#define OPTIONAL_CONFIGURATION_ENTRY(T)                                                                      \
    template ArgumentType<void(T)> optional_configuration_entry<ArgumentType<void(T)>>(                      \
        std::string const&, std::string const&, ArgumentType<void(T)> const&);                               \
                                                                                                             \
    template ArgumentType<void(T)> optional_configuration_entry<ArgumentType<void(T)>>(                      \
        std::string const&, ArgumentType<void(T)> const&);

    REQUIRED_CONFIGURATION_ENTRY(std::uint32_t);
    REQUIRED_CONFIGURATION_ENTRY(std::int32_t);
    REQUIRED_CONFIGURATION_ENTRY(std::uint64_t);
    REQUIRED_CONFIGURATION_ENTRY(std::int64_t);
    REQUIRED_CONFIGURATION_ENTRY(std::string);
    REQUIRED_CONFIGURATION_ENTRY(std::vector<std::uint64_t>);
    REQUIRED_CONFIGURATION_ENTRY(std::vector<std::int64_t>);
    REQUIRED_CONFIGURATION_ENTRY(ESC(lue::Shape<std::uint64_t, 2>));
    REQUIRED_CONFIGURATION_ENTRY(ESC(lue::Shape<std::int64_t, 2>));

    OPTIONAL_CONFIGURATION_ENTRY(std::uint32_t);
    OPTIONAL_CONFIGURATION_ENTRY(std::int32_t);
    OPTIONAL_CONFIGURATION_ENTRY(std::uint64_t);
    OPTIONAL_CONFIGURATION_ENTRY(std::int64_t);
    OPTIONAL_CONFIGURATION_ENTRY(std::string);
    OPTIONAL_CONFIGURATION_ENTRY(std::vector<std::uint64_t>);
    OPTIONAL_CONFIGURATION_ENTRY(std::vector<std::int64_t>);
    OPTIONAL_CONFIGURATION_ENTRY(ESC(lue::Shape<std::uint64_t, 2>));
    OPTIONAL_CONFIGURATION_ENTRY(ESC(lue::Shape<std::int64_t, 2>));

#undef REQUIRED_CONFIGURATION_ENTRY
#undef OPTIONAL_CONFIGURATION_ENTRY

}  // namespace lue
