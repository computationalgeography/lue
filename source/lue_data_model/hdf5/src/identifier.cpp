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

    : _id{-1},
      _close{nullptr}

{
    assert(!is_valid());
}


/*!
    @brief      Construct an instance based on an HDF5 identifier and a
                close function
*/
Identifier::Identifier(
    hid_t id,
    Close const& close)

    : _id{id},
      _close(close)

{
    assert_invariant();
}


/*!
    @brief      Copy construct an instance

    If the HDF5 identifier is valid, its reference count will be incremented.
*/
Identifier::Identifier(
    Identifier const& other)

    : _id{other._id},
      _close{other._close}

{
    if(is_valid()) {
        increment_reference_count();
    }

    assert_invariant();
}


/*!
    @brief      Move construct an instance

    The resulting instance will be valid when @a other is valid, otherwise it
    will be invalid. Once the new instance is constructed, @a other will be
    invalid.
*/
Identifier::Identifier(
    Identifier&& other)

    : _id{std::move(other._id)},
      _close{std::move(other._close)}

{
    other._id = -1;

    assert_invariant();
    assert(!other.is_valid());
}


/*!
    @brief      Destruct the instance

    If necessary, the close function is called.
*/
Identifier::~Identifier()
{
    close_if_necessary();
}


/*!
    @brief      Copy-assign @a other to this instance

    If necessary, the close function is called on the layered HDF5
    identifier before the assignment.
*/
Identifier& Identifier::operator=(
    Identifier const& other)
{
    // Copy-assign:
    // - Clean-up this instance
    // - Copy the other instance in

    close_if_necessary();

    _id = other._id;

    if(is_valid()) {
        increment_reference_count();
    }

    _close = other._close;

    assert_invariant();

    return *this;
}


Identifier& Identifier::operator=(
    Identifier&& other)
{
    // Move-assign:
    // - Clean-up this instance
    // - Move the other instance in

    close_if_necessary();

    _id = std::move(other._id);
    other._id = -1;

    _close = std::move(other._close);

    assert_invariant();
    assert(!other.is_valid());

    return *this;
}


int Identifier::reference_count() const
{
    assert(is_valid());

    auto count = ::H5Iget_ref(_id);

    if(count < 0) {
        throw std::runtime_error("Cannot retrieve object's reference count");
    }

    return count;
}


int Identifier::increment_reference_count()
{
    assert(is_valid());

    auto count = ::H5Iinc_ref(_id);

    if(count < 0) {
        throw std::runtime_error("Cannot increment object's reference count");
    }

    return count;
}


/*!
    @brief      Close the HDF5 object pointed to by the instance
*/
void Identifier::close_if_necessary()
{
    if(is_valid()) {
        assert(reference_count() > 0);
        _close(_id);
    }
}


void Identifier::assert_invariant() const
{
    assert(!(is_valid() && _close == nullptr));
}


/*!
    @brief      Return whether the identifier is valid
*/
bool Identifier::is_valid() const
{
    // Don't call assert_invariant here, since it uses is_valid in its
    // implementation...
    auto status = ::H5Iis_valid(_id);

    if(status < 0) {
        throw std::runtime_error(
            "Cannot determine whether identifier is valid");
    }

    return status > 0;
}


/*!
    @brief      Return the layered HDF5 identifier.
*/
Identifier::operator hid_t() const
{
    return _id;
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

    auto const nr_bytes = ::H5Iget_name(_id, nullptr, 0);

    std::string result(nr_bytes, 'x');

    /* nr_bytes = */ ::H5Iget_name(_id,
// This test seems correct (201402L corresponds with C++14, non-const data()
// is introduced in C++17), but non-const data() does not seem to be
// available in GCC-5 and 6.
// #if __cplusplus > 201402L
//         result.data()
// #else
//         const_cast<std::string::value_type*>(result.data())
// #endif
        const_cast<std::string::value_type*>(result.data())
        , nr_bytes + 1);

    return result;
}


/*!
    @brief      Return the name component of the pathname to the object
    @sa         pathname()

    The name is the part of the pathname after the last forward slash. If
    there is no forward slash in the pathname, name equals the pathname.

    An HDF5 object itself does not have a name. It has a unique ID within
    a file and at least one pathname pointing to it.
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

    return ObjectInfo(_id);
}


/*!
    @brief      Return whether two object identifiers are equal

    Two identifiers are considered equal if they are pointing to the same
    object in the HDF5 dataset.
*/
bool Identifier::operator==(
    Identifier const& other) const
{
    return info() == other.info();
}


/*!
    @brief      Return whether two object identifiers are not equal

    Two identifiers are considered equal if they are pointing to the same
    object in the HDF5 dataset.
*/
bool Identifier::operator!=(
    Identifier const& other) const
{
    return info() != other.info();
}

} // namespace hdf5
} // namespace lue
