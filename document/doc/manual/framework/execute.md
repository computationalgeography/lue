(execute-lue-framework-programs)=

# Execute LUE framework programs

Programs that use the LUE framework can be executed on a single node, like a laptop or desktop computer, or on
a distributed memory system, like a compute cluster.

LUE framework programs are designed to use the available hardware as good as possible. Which hardware is
available to the running program is determined by command line arguments.

The LUE framework makes use of the HPX AMT (asynchronous many-tasks) runtime, which can be configured using
configuration files and command line arguments. How this works is described in the [HPX
manual](https://hpx-docs.stellar-group.org/latest/html/manual/launching_and_configuring_hpx_applications.html).
Additionally, LUE framework programs provide LUE-specific command line arguments, for example for setting the
partition size for partitioned arrays.

The next sections provide examples for executing LUE framework programs. Unless stated, the information is
relevant for binary programs using the [LUE framework C++ API](#lue-framework-cpp-api), as well as for Python
scripts using the [LUE framework Python API](#lue-framework-python-api).

In the examples in this section, we assume that a cluster exists with nodes that have the following
properties:

|            |     |
| ---        | --- |
| CPUs       | 2 [AMD EPYC 7451](https://www.amd.com/en/products/cpu/amd-epyc-7451) (2 packages) |
| NUMA nodes | 8 (4 / package) |
| Cores      | 48 (6 / NUMA node) |

Each cluster node contains 2 CPUs, each of which contains 4 NUMA nodes, each of which contains 6 cores. One
easy way to make use of the fact that latencies to memory within a NUMA node are smaller than between them is
to assign a LUE framework program to each NUMA node (8 / cluster node).

````{note}
In case you experience crashes when running LUE framework programs, these may be related to the custom memory
allocator being loaded later than the program starts allocating memory. Memory allocations and deallocations
must be handled by the same allocation library. Depending on how the HPX library LUE depends upon is built, a
non-standard allocation library containing faster memory allocation and deallocation functions may be used. In
case of Python scripts using LUE, this allocation library will only be loaded once the script has already
started. We have noticed that in case HPX is built with support for the tcmalloc memory allocation library,
starting LUE Python scripts like this solved the crashes:

```bash
LD_PRELOAD=<prefix>/lib/libtcmalloc_minimal.so.4 python ./my_lue_script.py
```
````


## Single-node execution

TODO


## Distributed execution


### SLURM

See also [How to use HPX applications with
SLURM](https://hpx-docs.stellar-group.org/latest/html/manual/running_on_batch_systems.html#how-to-use-hpx-applications-with-slurm).


#### Synchronous

The example script shows how to start a synchronous SLURM job. This is handy for quick runs, when you are
still developing the model, for example.

The `UCX_LOG_LEVEL` environment variable and the `--mca btl_openib_allow_ib true` argument of mpirun are
necessary on the cluster on which this was tested. These are unrelated to LUE or HPX, and may or may not be
necessary on other clusters.

```bash
# Fixed. Depends on platform.
partition="my_partition"
nr_numa_domains_per_node=8
nr_cores_per_socket=6
nr_cpus_per_task=12
cpu_binding="thread:0-5=core:0-5.pu:0"

# Depends on size of job.
nr_nodes=1

# Fixed.
nr_localities=$(expr $nr_nodes \* $nr_numa_domains_per_node)

export UCX_LOG_LEVEL=error

salloc \
    --partition=$partition \
    --nodes=$nr_nodes \
    --ntasks=$nr_localities \
    --cpus-per-task=$nr_cpus_per_task \
    --cores-per-socket=$nr_cores_per_socket \
    mpirun \
        --mca btl_openib_allow_ib true \
        python my_model.py \
            my_argument1 my_argument2 \
            --hpx:ini="hpx.os_threads=$nr_cores_per_socket" \
            --hpx:bind=$cpu_binding \
            --hpx:print-bind
```

```{note}
In case you see this warning::

    hpx::init: command line warning: --hpx:localities used when running with SLURM, requesting a different
    number of localities (8) than have been assigned by SLURM (1), the application might not run properly.

but the printed CPU bindings seem fine, then you can safely ignore it.
```

#### Asynchronous

The example script shows how to start an asynchronous SLURM job. This is handy for long running runs, after
you have finished developing the model, for example.

```bash
# Fixed. Depends on platform.
partition="my_partition"
nr_numa_domains_per_node=8
nr_cores_per_socket=6
nr_cpus_per_task=12
cpu_binding="thread:0-5=core:0-5.pu:0"

# Depends on size of job.
nr_nodes=1

# Fixed.
nr_localities=$(expr $nr_nodes \* $nr_numa_domains_per_node)

# Depends on size of job
nr_nodes=12

# Fixed
nr_tasks=$(expr $nr_nodes \* $nr_numa_domains_per_node)

sbatch --job-name my_job_name << END_OF_SLURM_SCRIPT
#!/usr/bin/env bash
#SBATCH --nodes=$nr_nodes
#SBATCH --ntasks=$nr_localities
#SBATCH --cpus-per-task=$nr_cpus_per_task
#SBATCH --cores-per-socket=$nr_cores_per_socket
#SBATCH --partition=$partition

set -e

module purge
module load my_required_model

mpirun \
    --mca btl_openib_allow_ib true \
    python my_model.py \
        my_argument1 my_argument2 \
        --hpx:ini="hpx.os_threads=$nr_cores_per_socket" \
        --hpx:bind=$cpu_binding \
        --hpx:print-bind

END_OF_SLURM_SCRIPT
```
