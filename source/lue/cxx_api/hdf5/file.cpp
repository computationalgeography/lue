#include "lue/cxx_api/hdf5/file.h"
#include <cassert>


namespace lue {
namespace hdf5 {

File::File(
    Identifier&& id)

    : _id{std::forward<Identifier>(id)}

{
}


File::File(
    File&& other)

    : _id{std::move(other._id)}

{
    // Invalidate other.
    std::move(other);
}


File& File::operator=(
    File&& other)
{
    _id = std::move(other._id);

    // Invalidate other.
    std::move(other);

    return *this;
}


Identifier const& File::id() const
{
    return _id;
}

} // namespace hdf5
} // namespace lue
