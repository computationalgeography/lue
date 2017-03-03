#include "lue/hdf5/identifier.h"
#include <cassert>
#include <cstring>
#include <type_traits>


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


// /*!
//     @brief      Move-construct from the @a other instance.
// 
//     The @a other instance's state is invalid() after the move.
// */
// Identifier::Identifier(
//     Identifier&& other)
// 
//     : _id(std::move(other._id)),
//       _close(std::move(other._close))
// 
// {
//     // Invalidate other.
//     assert(!other._id);
//     other._close = nullptr;
// 
//     assert(!other.is_valid());
// }


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


// /*!
//     @brief      Move-assign from the @a other instance.
// 
//     The @a other instance's state is invalid() after the move.
// */
// Identifier& Identifier::operator=(
//     Identifier&& other)
// {
//     close_if_necessary();
//     _id = std::move(other._id);
//     _close = other._close;
// 
//     // Invalidate other.
//     assert(!other._id);
//     other._close = nullptr;
// 
//     assert(!other.is_valid());
// 
//     return *this;
// }


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
        ::H5Iis_valid(*_id) <= 0 ||

        // Valid id and close function.
        _close != nullptr
    );

    return _id && ::H5Iis_valid(*_id) > 0;
}


/*!
    @brief      Return the layered HDF5 identifier.
*/
Identifier::operator hid_t() const
{
    return *_id;
}


std::string Identifier::pathname() const
{
    static_assert(std::is_same<std::string::value_type, char>::value,
        "expect std::string::value_type to be char");

    assert(is_valid());
    auto nr_bytes = ::H5Iget_name(*_id, nullptr, 0);

    std::string result(nr_bytes, 'x');

    /* nr_bytes = */ ::H5Iget_name(*_id,
#if __cplusplus > 201402L
        result.data()
#else
        const_cast<std::string::value_type*>(result.data())
#endif
        , nr_bytes + 1);

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
