#pragma once
#include "lue/hdf5/configure.hpp"
#include "lue/hdf5/group.hpp"
#include "lue/hdf5/property_list.hpp"


namespace lue::hdf5 {

    /*!
        @brief      This class represents an HDF5 file
    */
    class LUE_HDF5_EXPORT File: public Group
    {

        public:

            class LUE_HDF5_EXPORT AccessPropertyList: public PropertyList
            {

                public:

                    AccessPropertyList();

                    void use_core_driver(std::size_t increment = 64000, ::hbool_t backing_store = 0);

#ifdef HDF5_IS_PARALLEL
                    void use_mpi_communicator(::MPI_Comm const& communicator, ::MPI_Info const& info);
#endif

                    void set_library_version_bounds(H5F_libver_t low, H5F_libver_t high);
            };

            explicit File(std::string const& name);

            File(std::string const& name, unsigned int flags);

            File(std::string const& name, AccessPropertyList const& access_property_list);

            File(std::string const& name, unsigned int flags, AccessPropertyList const& access_property_list);

            explicit File(Identifier&& id);

            explicit File(Group&& group);

            [[nodiscard]] auto hdf5_version() const -> std::string;

            [[nodiscard]] auto pathname() const -> std::string;

            void flush() const;

            [[nodiscard]] auto intent() const -> unsigned int;
    };


    LUE_HDF5_EXPORT auto file_exists(std::string const& name) -> bool;

    LUE_HDF5_EXPORT auto create_file(std::string const& name) -> File;

    LUE_HDF5_EXPORT auto create_file(
        std::string const& name, File::AccessPropertyList const& access_property_list) -> File;

    LUE_HDF5_EXPORT auto create_in_memory_file(std::string const& name) -> File;

    LUE_HDF5_EXPORT void remove_file(std::string const& name);

}  // namespace lue::hdf5
