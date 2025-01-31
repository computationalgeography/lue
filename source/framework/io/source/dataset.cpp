#include "lue/framework/io/dataset.hpp"
#include "lue/configure.hpp"


namespace lue {

    auto open_dataset(std::string const& pathname, unsigned int flags) -> data_model::Dataset
    {
        // Open dataset. Configure for use of parallel I/O if necessary.
        hdf5::File::AccessPropertyList access_property_list{};

#if LUE_FRAMEWORK_WITH_PARALLEL_IO
        // Open file collectively (does not imply synchronicity!)
        MPI_Comm communicator{MPI_COMM_WORLD};
        MPI_Info info{MPI_INFO_NULL};
        access_property_list.use_mpi_communicator(communicator, info);
#endif

        return data_model::open_dataset(pathname, flags, access_property_list);
    }

}  // namespace lue
