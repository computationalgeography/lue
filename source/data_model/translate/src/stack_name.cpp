#include "lue/translate/stack_name.hpp"
#include <fmt/format.h>


namespace lue {
namespace utility {

StackName::StackName(
    std::string const& name):

    _parent_path{std::filesystem::path(name).parent_path()},
    _stem{std::filesystem::path(name).stem()},
    _extension{std::filesystem::path(name).extension()}

{
}


std::string StackName::operator[](
    std::size_t const idx) const
{
    return fmt::format(
        "{}_{}{}", (
        _parent_path / _stem).generic_string(), idx, _extension.string());
}

}  // namespace utility
}  // namespace lue
