#include "lue/utility/environment.hpp"
#include <regex>


namespace lue {

std::string expand_environment_variables(
    std::string string)
{
    static std::regex const expression{R"(\$\{([^}]+)\})"};
    std::smatch match_results;

    // Name of variable found in string
    std::string variable_name;

    // Pointer to environment variable value, possibly nullptr
    char* variable_value_ptr;

    // Value of environment variable, possibly empty
    std::string variable_value;

    // Index of first character of range of characters to search in
    std::size_t idx = 0;

    while(std::regex_search(
            string.cbegin() + idx, string.cend(),
            match_results, expression))
    {

        // A variable name was found in the range of characters
        auto const& full_match = match_results[0];
        variable_name = match_results[1].str();

        variable_value_ptr = std::getenv(variable_name.c_str());
        variable_value = variable_value_ptr != nullptr
            ? std::string{variable_value_ptr} : std::string{};

        // Replace variable name with, possibly empty, value
        string.replace(full_match.first, full_match.second, variable_value);

        // Set index to character just after the value just inserted
        idx += variable_value.size();
    }

    return string;
}

}  // namespace lue
