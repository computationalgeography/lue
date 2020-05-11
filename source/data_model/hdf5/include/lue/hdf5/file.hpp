#pragma once
#include "lue/hdf5/configure.hpp"
#include "lue/hdf5/group.hpp"
#include "lue/hdf5/property_list.hpp"


namespace lue {
namespace hdf5 {

/*!
    @brief      This class represents an HDF5 file
*/
class File:
    public Group
{

public:

    class AccessPropertyList:
        public PropertyList
    {

    public:

                   AccessPropertyList  ();

        void       use_core_driver     (std::size_t increment=64000,
                                        hbool_t backing_store=0);

#ifdef HDF5_IS_PARALLEL
        void       use_mpi_communicator(MPI_Comm const& communicator,
                                        MPI_Info const& info);
#endif

        void       set_library_version_bounds(
                                        H5F_libver_t low,
                                        H5F_libver_t high);

    };

    explicit       File                (std::string const& name);

                   File                (std::string const& name,
                                        unsigned int flags);

                   File                (std::string const& name,
                                        AccessPropertyList const&
                                            access_property_list);

                   File                (std::string const& name,
                                        unsigned int flags,
                                        AccessPropertyList const&
                                            access_property_list);

    explicit       File                (Identifier&& id);

    explicit       File                (Group&& group);

                   File                (File const&)=delete;

                   File                (File&&)=default;

                   ~File               () override =default;

    File&          operator=           (File const&)=delete;

    File&          operator=           (File&&)=default;

    std::string    hdf5_version        () const;

    std::string    pathname            () const;

    void           flush               () const;

    unsigned int   intent              () const;

private:

};


bool               file_exists         (std::string const& name);

File               create_file         (std::string const& name,
                                        File::AccessPropertyList const&
                                            access_property_list);

File               create_file         (std::string const& name);

File               create_in_memory_file(
                                        std::string const& name);

void               remove_file         (std::string const& name);

} // namespace hdf5
} // namespace lue
