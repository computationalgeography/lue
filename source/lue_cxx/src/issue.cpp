#include "lue/issue.hpp"


namespace lue {

/*!
    @brief      Construct issue based on @a id of HDF5 object and @a message
*/
Issue::Issue(
    hdf5::Identifier const& id,
    std::string const& message)

    : _id(id),
      _message(message)

{
}


/*!
    @brief      Return id of HDF5 object related to the issue
*/
hdf5::Identifier const& Issue::id() const
{
    return _id;
}


/*!
    @brief      Return message describing the issue
*/
std::string const& Issue::message() const
{
    return _message;
}

}  // namespace lue
