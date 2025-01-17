#pragma once
#include "lue/framework/io/from_lue.hpp"
#include "lue/framework/io/gdal.hpp"
#include "lue/framework/io/to_lue.hpp"


/*!
    @file

    Asynchronous I/O can be tricky. In general, the functions that read and write data try to do this
    completely asynchronous, meaning that the functions return as quickly as possible, while the actual I/O
    happens in spawned tasks, once scheduled for execution. When reading partitioned data, partitions are
    likely to be marked as ready individually so that further processing can already start for those
    partitions.

    Things to look out for:

    - If all data has been read (e.g. all partitions are marked as ready), this does not mean that the dataset
      is closed. Opening the same dataset for writing might fail. Waiting for returned futureѕ to be ready
      does not prevend this.

    Issues have popped up in unit tests. It may well be that in the real world these issues are unlikely to be
    relevant. For example, in unit test we are often dealing with very small datasets and fast iterations.
    This results in program states that are different from normal cases when datasets are larger.


    # HDF5

    We can read from a dataset from multiple localities (processes) at the same time. We cannot
    read from a dataset from multiple threads in a single process at the same time. We cannot
    assume that the HDF5 library is thread safe.

    - Reads into partitions located in different localities don't have to be serialized
    - Reads into partitions located in the same locality must be serialized
*/
