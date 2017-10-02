#include "lue/hdf5/file.hpp"
#include <boost/filesystem.hpp>
#include <cassert>


namespace lue {
namespace hdf5 {

File::AccessPropertyList::AccessPropertyList()

    : PropertyList(H5P_FILE_ACCESS)

{
}


/*!
    @brief      Use the H5D_CORE driver
    @exception  std::runtime_error In case the H5D_CORE driver cannot be set
    @sa         [H5Pset_fapl_core](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5P.html#Property-SetFaplCore)
*/
void File::AccessPropertyList::use_core_driver(
    std::size_t const increment,
    hbool_t const backing_store)
{
    // size_t const increment = 64000;  // 64k
    // hbool_t const backing_store = 0;  // false

    auto status = ::H5Pset_fapl_core(id(), increment, backing_store);

    if(status < 0) {
        throw std::runtime_error("Cannot set core file driver");
    }
}


#ifdef HDF5_IS_PARALLEL
/*!
    @brief      Use MPI communicator for creating/opening a file
    @exception  std::runtime_error In case the communicator information cannot
                be set
    @sa         [H5Pset_fapl_mpio](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5P.html#Property-SetFaplMpio)
*/
void File::AccessPropertyList::use_mpi_communicator(
    MPI_Comm const& communicator,
    MPI_Info const& info)
{
    auto status = ::H5Pset_fapl_mpio(id(), communicator, info);

    if(status < 0) {
        throw std::runtime_error("Cannot set MPI communicator");
    }
}
#endif


/*!
    @brief      Open file
    @param      name Name of file
    @param      flags File access flags: H5F_ACC_RDWR, H5F_ACC_RDONLY
    @exception  std::runtime_error In case the file cannot be opened
*/
File::File(
    std::string const& name,
    unsigned int const flags)

    : File(name, flags, AccessPropertyList())

    // : Group(Identifier(::H5Fopen(name.c_str(), flags, H5P_DEFAULT),
    //     ::H5Fclose))

{
    // if(!id().is_valid()) {
    //     throw std::runtime_error("Cannot open file " + name);
    // }
}


/*!
    @brief      Open file in read-only mode
    @param      name Name of file
    @exception  std::runtime_error In case the file cannot be opened
*/
File::File(
    std::string const& name)

    : File(name, H5F_ACC_RDONLY, AccessPropertyList())

{
}


File::File(
    std::string const& name,
    unsigned int const flags,
    AccessPropertyList const& access_property_list)

    : Group(Identifier(::H5Fopen(name.c_str(), flags,
        access_property_list.id()), ::H5Fclose))

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


void File::flush() const
{
    auto status = ::H5Fflush(id(), H5F_SCOPE_LOCAL);

    if(status < 0) {
        throw std::runtime_error("Cannot flush file " + pathname());
    }
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
    @brief      Create an in-memory file
*/
File create_in_memory_file(
    std::string const& name)
{
    auto access_property_list = File::AccessPropertyList();

    access_property_list.use_core_driver();

    Identifier id(::H5Fcreate(name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT,
        access_property_list.id()), ::H5Fclose);

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
