#include "lue/framework/io/dataset.hpp"
// #include "lue/framework/io/configure.hpp"  // LUE_USE_PARALLEL_IO
#include "lue/configure.hpp"


namespace lue {

    auto open_dataset(std::string const& pathname, unsigned int flags) -> data_model::Dataset
    {
        // Open dataset. Configure for use of parallel I/O if necessary.
        hdf5::File::AccessPropertyList access_property_list{};

#if LUE_FRAMEWORK_WITH_PARALLEL_IO
        // TODO
        // Each locality must collectively call open dataset → this blocks / is a barrier
        // Does this block until all *processes* have done this call? Is this determined automatically
        // behind the scenes, by MPI?

        // KDJ: Old stuff, never worked, replace by something that does
        // if(hpx::util::mpi_environment::enabled())
        // {
        //     // ::MPI_Comm communicator{hpx::util::mpi_environment::communicator()};
        //     ::MPI_Comm communicator{MPI_COMM_WORLD};
        //     ::MPI_Info info{MPI_INFO_NULL};
        //     access_property_list.use_mpi_communicator(communicator, info);
        // }
#endif

        return data_model::open_dataset(pathname, flags, access_property_list);
    }

}  // namespace lue
