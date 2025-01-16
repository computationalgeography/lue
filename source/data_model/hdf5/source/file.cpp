#include "lue/hdf5/file.hpp"
#include "lue/hdf5/version.hpp"
#include <fmt/format.h>
#include <cassert>
#include <filesystem>


namespace lue::hdf5 {

    File::AccessPropertyList::AccessPropertyList():

        PropertyList{Identifier{H5Pcreate(H5P_FILE_ACCESS), H5Pclose}}

    {
    }


    /*!
        @brief      Use the H5D_CORE driver
        @param      backing_store Whether or not the file is written to disk
                    upon close
        @exception  std::runtime_error In case the H5D_CORE driver cannot be set
        @sa [H5Pset_fapl_core](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5P.html#Property-SetFaplCore)
    */
    void File::AccessPropertyList::use_core_driver(std::size_t const increment, ::hbool_t const backing_store)
    {
        // std::size_t const increment = 64000;  // 64k
        // hbool_t const backing_store = 0;  // false

        herr_t const status{::H5Pset_fapl_core(id(), increment, backing_store)};

        if (status < 0)
        {
            throw std::runtime_error("Cannot set core file driver");
        }
    }


#ifdef HDF5_IS_PARALLEL
    /*!
        @brief      Use MPI communicator for creating/opening a file
        @exception  std::runtime_error In case the communicator information cannot
                    be set
        @sa [H5Pset_fapl_mpio](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5P.html#Property-SetFaplMpio)
    */
    void File::AccessPropertyList::use_mpi_communicator(
        ::MPI_Comm const& communicator, ::MPI_Info const& info)
    {
        ::herr_t const status{::H5Pset_fapl_mpio(id(), communicator, info)};

        if (status < 0)
        {
            throw std::runtime_error("Cannot set MPI communicator");
        }
    }
#endif


    /*!
        @brief      Set bounds on library versions, and indirectly format
                    versions, to be used when creating objects
        @param      low Earliest version of library that will be used for
                    writing objects
        @param      high ...
        @exception  std::runtime_error In case the bounds on the library
                    versions cannot be set
        @sa
       [H5Pset_libver_bounds](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5P.html#Property-SetLibverBounds)
    */
    void File::AccessPropertyList::set_library_version_bounds(
        ::H5F_libver_t const low, ::H5F_libver_t const high)
    {
        ::herr_t const status{::H5Pset_libver_bounds(id(), low, high)};

        if (status < 0)
        {
            throw std::runtime_error("Cannot set library version bounds");
        }
    }


    /*!
        @brief      Open file
        @param      name Name of file
        @param      flags File access flags: H5F_ACC_RDWR, H5F_ACC_RDONLY
        @exception  std::runtime_error In case the file cannot be opened
    */
    File::File(std::string const& name, unsigned int const flags):

        File{name, flags, AccessPropertyList{}}

    {
    }


    /*!
        @brief      Open file in read-only mode
        @param      name Name of file
        @exception  std::runtime_error In case the file cannot be opened
    */
    File::File(std::string const& name):

        File{name, H5F_ACC_RDONLY, AccessPropertyList{}}

    {
    }


    File::File(std::string const& name, AccessPropertyList const& access_property_list):

        File{name, H5F_ACC_RDONLY, access_property_list}

    {
    }


    File::File(
        std::string const& name, unsigned int const flags, AccessPropertyList const& access_property_list):

        Group{Identifier(::H5Fopen(name.c_str(), flags, access_property_list.id()), ::H5Fclose)}

    {
        if (!id().is_valid())
        {
            throw std::runtime_error(fmt::format("Cannot open file {}", name));
        }
    }


    File::File(Identifier&& id):

        Group{std::move(id)}

    {
    }


    File::File(Group&& group):

        Group{std::move(group)}

    {
    }


    std::string File::hdf5_version() const
    {
        return attribute<std::string>("hdf5_version");
    }


    std::string File::pathname() const
    {
        static_assert(
            std::is_same<std::string::value_type, char>::value, "expect std::string::value_type to be char");

        assert(id().is_valid());
        ::ssize_t const nr_bytes{::H5Fget_name(id(), nullptr, 0)};

        std::string result(nr_bytes, 'x');

        /* nr_bytes = */ ::H5Fget_name(id(), result.data(), nr_bytes + 1);

        return result;
    }


    void File::flush() const
    {
        ::herr_t const status{::H5Fflush(id(), H5F_SCOPE_LOCAL)};

        if (status < 0)
        {
            throw std::runtime_error(fmt::format("Cannot flush file {}", pathname()));
        }
    }


    unsigned int File::intent() const
    {
        unsigned int intent{};
        ::herr_t const status{::H5Fget_intent(id(), &intent)};

        if (status < 0)
        {
            throw std::runtime_error(fmt::format("Cannot determine intent of file {}", pathname()));
        }

        return intent;
    }


    // std::string File::name() const
    // {
    //     auto const pathname = this->pathname();
    //     auto const idx = pathname.find_last_of('/');
    //
    //     return idx == std::string::npos ? pathname : pathname.substr(idx + 1);
    // }


    /*!
        @brief      Return whether or not a file exists
        @param      name Name of dataset
        @warning    This function only checks whether a regular file named
                    @a name is present. No attempt is made to verify the file
                    is accessible.
    */
    bool file_exists(std::string const& name)
    {
        auto const status = std::filesystem::status(name);

        return std::filesystem::is_regular_file(status);
    }


    File create_file(std::string const& name, File::AccessPropertyList const& access_property_list)
    {
        Identifier id{
            ::H5Fcreate(name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, access_property_list.id()), ::H5Fclose};

        if (!id.is_valid())
        {
            throw std::runtime_error(fmt::format("Cannot create file {}", name));
        }

        File file{std::move(id)};

        file.attributes().write<std::string>("hdf5_version", hdf5_version());

        return file;
    }


    File create_file(std::string const& name)
    {
        // Pass in default access property list
        return create_file(name, File::AccessPropertyList{});
    }


    /*!
        @brief      Create an in-memory file
    */
    File create_in_memory_file(std::string const& name)
    {
        // Pass in access property list for in-memory access
        File::AccessPropertyList access_property_list{};

        access_property_list.use_core_driver();

        return create_file(name, access_property_list);
    }


    /*!
        @brief      Remove file
        @param      name Name of file
        @exception  std::runtime_error In case the file cannot be removed
    */
    void remove_file(std::string const& name)
    {
        std::filesystem::remove(name);
    }

}  // namespace lue::hdf5
