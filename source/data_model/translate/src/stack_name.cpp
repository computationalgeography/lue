#include "lue/translate/stack_name.hpp"
#include <fmt/format.h>


namespace lue {
namespace utility {

StackName::StackName(
    std::string const& name):

    _parent_path{boost::filesystem::path(name).parent_path()},
    _stem{boost::filesystem::path(name).stem()},
    _extension{boost::filesystem::path(name).extension()}

{
}


std::string StackName::operator[](
    std::size_t const idx) const
{
    return fmt::format(
        "{}_{}{}", (
        _parent_path / _stem).string(), idx, _extension.string());
}

}  // namespace utility
}  // namespace lue
