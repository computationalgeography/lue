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

    : _id(std::make_shared<hid_t>(id)),
      _close(close)

{
}


Identifier::Identifier(
    Identifier const& other)

    : _id(other._id),
      _close(other._close)

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
    assert(!other._id);
    other._close = nullptr;

    assert(!other.is_valid());
}


/*!
    @brief      Destruct the instance.

    The close function is called on valid identifiers.
*/
Identifier::~Identifier()
{
    close_if_necessary();
}


Identifier& Identifier::operator=(
    Identifier const& other)
{
    close_if_necessary();
    _id = other._id;
    _close = other._close;

    return *this;
}


/*!
    @brief      Move-assign from the @a other instance.

    The @a other instance's state is invalid() after the move.
*/
Identifier& Identifier::operator=(
    Identifier&& other)
{
    close_if_necessary();
    _id = std::move(other._id);
    _close = other._close;

    // Invalidate other.
    assert(!other._id);
    other._close = nullptr;

    assert(!other.is_valid());

    return *this;
}


void Identifier::close_if_necessary()
{
    if(is_valid() && _id.unique()) {
        assert(_close != nullptr);
        _close(*_id);
    }
}


/*!
    @brief      Return whether the identifier is valid.
*/
bool Identifier::is_valid() const
{
    // Invariants.
    assert(
        // 'Empty' state.
        (!_id && _close == nullptr) ||

        // Invalid id. Close function won't be used.
        hdf5_is_valid(*_id) <= 0 ||

        // Valid id and close function.
        _close != nullptr
    );

    return _id && hdf5_is_valid(*_id) > 0;
}


/*!
    @brief      Return the layered HDF5 identifier.
*/
Identifier::operator hid_t() const
{
    // The caller should guard this call by is_valid().
    assert(is_valid());

    return *_id;
}


std::string Identifier::pathname() const
{
    char* name;

    auto nr_bytes = hdf5_name(*_id, &name);

    std::string result;

    if(nr_bytes > 0) {
        result = name;
        ::free(name);
    }

    return result;
}


std::string Identifier::name() const
{
    auto const pathname = this->pathname();
    auto const idx = pathname.find_last_of('/');

    return idx == std::string::npos ? pathname : pathname.substr(idx + 1);
}

} // namespace hdf5
} // namespace lue
