#include "lue/translate/stack_name.hpp"
#include <format>


namespace lue::utility {

    StackName::StackName(std::string const& name):

        _parent_path{std::filesystem::path(name).parent_path()},
        _stem{std::filesystem::path(name).stem()},
        _extension{std::filesystem::path(name).extension()}

    {
    }


    auto StackName::operator[](std::size_t const idx) const -> std::string
    {
        return std::format("{}_{}{}", (_parent_path / _stem).generic_string(), idx, _extension.string());
    }

}  // namespace lue::utility
