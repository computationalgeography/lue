#pragma once
#include "lue/framework/configure.hpp"
#include "lue/hdf5/configure.hpp"


#if defined(LUE_HPX_WITH_MPI) && defined(HDF5_IS_PARALLEL)
#define LUE_USE_PARALLEL_IO
#endif
