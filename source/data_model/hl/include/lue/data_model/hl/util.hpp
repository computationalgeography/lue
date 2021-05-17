#pragma once
#include <array>
#include <string>


namespace lue {

    std::array<std::string, 4> parse_array_pathname(std::string const& array_pathname);

    std::array<std::string, 3> parse_array_pathname3(std::string const& array_pathname);

}  // namespace lue
