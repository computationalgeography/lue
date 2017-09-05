#include "lue/validate/issues.hpp"


namespace lue {
namespace utility {

Errors const& Issues::errors() const
{
    return _errors;
}


Warnings const& Issues::warnings() const
{
    return _warnings;
}


void Issues::add_error(
    hdf5::Identifier const& id,
    std::string const& message)
{
    _errors.emplace_back(Issue(id, message));
}


void Issues::add_warning(
    hdf5::Identifier const& id,
    std::string const& message)
{
    _warnings.emplace_back(Issue(id, message));
}


bool Issues::errors_found() const
{
    return !_errors.empty();
}


bool Issues::warnings_found() const
{
    return !_warnings.empty();
}

}  // namespace utility
}  // namespace lue
