#include "lue/cxx_api/hdf5/file.h"
#include "lue/c_api/hdf5/hdf5_file.h"
#include <cassert>


namespace lue {
namespace hdf5 {

File::File(
    Identifier&& id)

    : Group(std::forward<Identifier>(id))

{
}


File::File(
    File&& other)

    : Group(std::forward<Group>(other))

{
}


File& File::operator=(
    File&& other)
{
    Group::operator=(std::forward<Group>(other));

    return *this;
}


std::string File::pathname() const
{
    char* name;

    auto nr_bytes = hdf5_file_pathname(id(), &name);

    std::string result;

    if(nr_bytes > 0) {
        result = name;
        ::free(name);
    }

    return result;
}


File open_file(
    std::string const& name)
{
    hdf5::Identifier file_id(
        ::H5Fopen(name.c_str(), H5F_ACC_RDWR, H5P_DEFAULT),
        ::H5Fclose);

    if(!file_id.is_valid()) {
        throw std::runtime_error("File " + name + " cannot be opened");
    }

    return File(std::move(file_id));
}

} // namespace hdf5
} // namespace lue
