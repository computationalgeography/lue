#include "detail/unsupported_overload.hpp"
#include <fmt/format.h>


namespace lue::detail {

    void unsupported_overload(std::string const& name, std::string const& field1, std::string const& field2)
    {
        throw std::runtime_error(fmt::format("Invalid overload called: {}({}, {})", name, field1, field2));
    }

}  // namespace lue::detail
