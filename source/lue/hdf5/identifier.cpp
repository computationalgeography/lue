#include "lue/hdf5/identifier.h"
#include <cassert>
#include <cstring>
#include <type_traits>


namespace lue {
namespace hdf5 {

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
    @brief      Close the identifier if necessary

    The close function will be called if:
    - the instance is being destructed (this is assumed to be true)
    - the identifier is valid
    - we are the only ones using it
*/
void Identifier::close_if_necessary()
{
    if(is_valid() && _id.unique()) {
        assert(_close != nullptr);
        auto status = _close(*_id);
        assert(status >= 0);

        // Why is this not the case???
        // assert(!is_valid());
    }
}


/*!
    @brief      Return whether the identifier is valid
*/
bool Identifier::is_valid() const
{
    // Invariants.
    assert(
        // 'Empty' state.
        ((!_id) && _close == nullptr) ||

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
    auto const pathname = this->pathname();
    auto const idx = pathname.find_last_of('/');

    return idx == std::string::npos ? pathname : pathname.substr(idx + 1);
}

} // namespace hdf5
} // namespace lue
