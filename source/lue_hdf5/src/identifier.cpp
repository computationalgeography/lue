#include "lue/hdf5/identifier.hpp"
#include <cassert>
#include <cstring>
#include <type_traits>


namespace lue {
namespace hdf5 {

/*!
    @brief      Default construct an instance
    @sa         is_valid()

    The resulting instance will not be valid.
*/
Identifier::Identifier()

    : _id{},
      _close{nullptr}

{
    assert(is_empty());
    assert_invariant();
}


/*!
    @brief      Construct an instance based on an HDF5 identifier and a
                close function
*/
Identifier::Identifier(
    hid_t id,
    Close const& close)

    : _id(std::make_shared<hid_t>(id)),
      _close(close)

{
    assert_invariant();
}


/*!
    @brief      Destruct the instance

    If necessary, the close function is called.

    The close function will be called if:
    - the identifier is valid
    - we are the only ones using it
*/
Identifier::~Identifier()
{
    assert_invariant();

    close_if_necessary();

    assert_invariant();
}


Identifier& Identifier::operator=(
    Identifier const& other)
{
    assert_invariant();

    close_if_necessary();
    _id = other._id;
    _close = other._close;

    assert_invariant();

    return *this;
}


Identifier& Identifier::operator=(
    Identifier&& other)
{
    assert_invariant();

    close_if_necessary();
    _id = std::move(other._id);
    _close = std::move(other._close);

    assert_invariant();

    return *this;
}


/*!
    @brief      Close the HDF5 object pointed to by the instance
*/
void Identifier::close_if_necessary()
{
    if(is_valid() && _id.unique()) {
#ifndef NDEBUG
        auto status =
#endif
            _close(*_id);
        assert(status >= 0);

        _id.reset();
        _close = nullptr;
    }
}


void Identifier::assert_invariant() const
{
    // An identifier is either empty, іnvalid, or valid.
    // - An empty identifier has no id. In that case, the close function
    //   must not be set.
    // - An invalid identifier has an invalid id and a close function.
    // - A valid identifier has a valid id and a close function.
    assert((is_empty() && _close == nullptr) || _close != nullptr);
}


bool Identifier::is_empty() const
{
    return !_id;
}


/*!
    @brief      Return whether the identifier is valid
*/
bool Identifier::is_valid() const
{
    assert_invariant();

    return !is_empty() && ::H5Iis_valid(*_id) > 0;
}


/*!
    @brief      Return the layered HDF5 identifier.
*/
Identifier::operator hid_t() const
{
    assert(!is_empty());

    return *_id;
}


/*!
    @brief      Return the pathname to the object
    @sa         name()

    There may be more than one pathname to an object. This function returns
    one of them. When possible, it is the one with which the object was
    opened.
*/
std::string Identifier::pathname() const
{
    static_assert(std::is_same<std::string::value_type, char>::value,
        "expect std::string::value_type to be char");

    assert(is_valid());

    auto const nr_bytes = ::H5Iget_name(*_id, nullptr, 0);

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


/*!
    @brief      Return the name of the object
    @sa         pathname()

    The name is the part of the pathname after the last forward slash. If
    there is no forward slash in the pathname, name equals the pathname.

    An object may have multiple names. This function returns
    one of them. When possible, it is the one with which the object was
    opened.
*/
std::string Identifier::name() const
{
    assert(is_valid());

    auto const pathname = this->pathname();
    auto const idx = pathname.find_last_of('/');

    return idx == std::string::npos ? pathname : pathname.substr(idx + 1);
}


ObjectInfo Identifier::info() const
{
    assert(is_valid());

    return ObjectInfo(*_id);
}


/*!
    @brief      Return whether two object identifiers are equal

    Two identifiers are considered equal if they are pointing to the same
    object in the HDF5 dataset.
*/
bool operator==(
    Identifier const& lhs,
    Identifier const& rhs)
{
    return lhs.info() == rhs.info();
}


/*!
    @brief      Return whether two object identifiers are not equal
    @sa         operator==(Identifier const&, Identifier const&)
*/
bool operator!=(
    Identifier const& lhs,
    Identifier const& rhs)
{
    return !(lhs == rhs);
}

} // namespace hdf5
} // namespace lue
