#include "lue/cxx_api/hdf5/file.h"
#include "lue/c_api/hdf5/hdf5_file.h"
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


std::string File::pathname() const
{
    char* name;

    auto nr_bytes = hdf5_file_pathname(_id, &name);

    std::string result;

    if(nr_bytes > 0) {
        result = name;
        ::free(name);
    }

    return result;
}

} // namespace hdf5
} // namespace lue
