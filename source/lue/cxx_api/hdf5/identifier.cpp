#include "lue/cxx_api/hdf5/identifier.h"
#include "lue/c_api/hdf5/hdf5_identifier.h"
#include <cassert>
#include <cstring>


namespace lue {
namespace hdf5 {

/*!
    @brief      Construct an instance based on an HDF5 identifier and a
                close function.
*/
Identifier::Identifier(
    hid_t id,
    Close const& close)

    : _id(id),
      _close(close)

{
}


/*!
    @brief      Move-construct from the @a other instance.

    The @a other instance's state is invalid() after the move.
*/
Identifier::Identifier(
    Identifier&& other)

    : _id(std::move(other._id)),
      _close(std::move(other._close))

{
    // Invalidate other.
    other._id = -1;
    other._close = nullptr;

    assert(!other.is_valid());
}


/*!
    @brief      Move-assign from the @a other instance.

    The @a other instance's state is invalid() after the move.
*/
Identifier& Identifier::operator=(
    Identifier&& other)
{
    if(is_valid()) {
        _close(_id);
    }

    _id = other._id;
    _close = other._close;

    // Invalidate other.
    other._id = -1;
    other._close = nullptr;

    assert(!other.is_valid());

    return *this;
}


/*!
    @brief      Destruct the instance.

    The close function is called on valid identifiers.
*/
Identifier::~Identifier()
{
    // Only close identifiers with valid ids.
    if(is_valid()) {
        assert(_close != nullptr);
        _close(_id);
    }
}


/*!
    @brief      Return whether the identifier is valid.
*/
bool Identifier::is_valid() const
{
    // In case an identifier is valid, the close function must be set.
    assert(hdf5_is_valid(_id) <= 0 || _close != nullptr);

    return hdf5_is_valid(_id) > 0;
}


/*!
    @brief      Return the layered HDF5 identifier.
*/
Identifier::operator hid_t() const
{
    // The caller should guard this call by is_valid().
    assert(is_valid());

    return _id;
}


std::string Identifier::pathname() const
{
    char* name;

    auto nr_bytes = hdf5_name(_id, &name);

    std::string result;

    if(nr_bytes > 0) {
        result = name;
        ::free(name);
    }

    return result;
}

} // namespace hdf5
} // namespace lue
