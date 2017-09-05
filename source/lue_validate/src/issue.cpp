#include "lue/validate/issue.hpp"


namespace lue {
namespace utility {

Issue::Issue(
    hdf5::Identifier const& id,
    std::string const& message)

    : _id(id),
      _message(message)

{
}


hdf5::Identifier const& Issue::id() const
{
    return _id;
}


std::string const& Issue::message() const
{
    return _message;
}

}  // namespace utility
}  // namespace lue
