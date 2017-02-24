#include "lue/hdf5/file.h"
#include <boost/filesystem.hpp>
#include <cassert>


namespace lue {
namespace hdf5 {

/*!
    @brief      Open file
    @param      name Name of file
    @param      flags File access flags: H5F_ACC_RDWR, H5F_ACC_RDONLY
    @exception  std::runtime_error In case the file cannot be opened
*/
File::File(
    std::string const& name,
    unsigned int const flags)

    : Group(Identifier(::H5Fopen(name.c_str(), flags, H5P_DEFAULT),
        ::H5Fclose))

{
    if(!id().is_valid()) {
        throw std::runtime_error("Cannot open file " + name);
    }
}


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
    static_assert(std::is_same<std::string::value_type, char>::value,
        "expect std::string::value_type to be char");

    assert(id().is_valid());
    auto nr_bytes = ::H5Fget_name(id(), nullptr, 0);

    std::string result(nr_bytes, 'x');

    /* nr_bytes = */ ::H5Fget_name(id(),
#if __cplusplus > 201402L
        result.data()
#else
        const_cast<std::string::value_type*>(result.data())
#endif
        , nr_bytes + 1);

    return result;
}


// std::string File::name() const
// {
//     auto const pathname = this->pathname();
//     auto const idx = pathname.find_last_of('/');
// 
//     return idx == std::string::npos ? pathname : pathname.substr(idx + 1);
// }


// /*!
//     @brief      .
//     @param      flags File access flags: H5F_ACC_RDWR, H5F_ACC_RDONLY
//     @return     .
//     @exception  .
// */
// File open_file(
//     std::string const& name,
//     unsigned int const flags)
// {
//     Identifier file_id(::H5Fopen(name.c_str(), flags, H5P_DEFAULT),
//         ::H5Fclose);
// 
//     if(!file_id.is_valid()) {
//         throw std::runtime_error("Cannot open file " + name);
//     }
// 
//     return File(std::move(file_id));
// }


/*!
    @brief      Return whether or not a file exists
    @param      name Name of dataset
    @warning    This function only checks whether a regular file named
                @a name is present. No attempt is made to verify the file
                is accessible.
*/
bool file_exists(
    std::string const& name)
{
    auto const status = boost::filesystem::status(name);

    return boost::filesystem::is_regular_file(status);
}


File create_file(
    std::string const& name)
{
    Identifier id(::H5Fcreate(name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT,
        H5P_DEFAULT), ::H5Fclose);

    if(!id.is_valid()) {
        throw std::runtime_error("Cannot create file " + name);
    }

    return File(std::move(id));
}


/*!
    @brief      Remove file
    @param      name Name of file
    @exception  std::runtime_error In case the file cannot be removed
*/
void remove_file(
    std::string const& name)
{
    boost::filesystem::remove(name);
}

} // namespace hdf5
} // namespace lue
