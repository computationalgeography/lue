#include "lue/utility/lue_validate/error.h"
#include <iostream>


namespace lue {

Error::Error(
    hdf5::Identifier const& id,
    std::string const& message)

    : _id(id),
      _message(message)

{
}


hdf5::Identifier const& Error::id() const
{
    return _id;
}


std::string const& Error::message() const
{
    return _message;
}

}  // namespace lue
