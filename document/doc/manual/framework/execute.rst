.. _execute_lue_framework_programs:

Execute LUE framework programs
==============================
Programs that use the LUE framework can be executed on a single node,
like a laptop or desktop computer, or on a distributed memory system,
like a compute cluster.

LUE framework programs are designed to use the available hardware as
good as possible. Which hardware is available to the running program is
determined by command line arguments.

The LUE framework makes use of the HPX AMT (asynchronous many-tasks)
runtime, which can be configured using configuration files and command
line arguments. How this works is described in the
`HPX manual`_. Additionally, LUE framework programs provide LUE-specific
command line arguments, for example for setting the partition size for
partitioned arrays.

.. _HPX manual: https://hpx-docs.stellar-group.org/latest/html/manual/launching_and_configuring_hpx_applications.html

The next sections provide examples for executing LUE framework
programs. Unless stated, the information is relevant for binary
programs using the :ref:`LUE framework C++ API <lue_framework_cpp_api>`,
as well as for Python scripts using the :ref:`LUE framework Python API
<lue_framework_python_api>`.

In the examples in this section, we assume that a cluster exists with
nodes that have the following properties:

========== ===============================
========== ===============================
CPUs       2 `AMD EPYC 7451`_ (2 packages)
NUMA nodes 8 (4 / package)
Cores      48 (6 / NUMA node)
========== ===============================

.. _AMD EPYC 7451: https://www.amd.com/en/products/cpu/amd-epyc-7451

Each cluster node contains 2 CPUs, each of which contains 4 NUMA nodes,
each of which contains 6 cores. One easy way to make use of the fact
that latencies to memory within a NUMA node are smaller than between
them is to assign a LUE framework program to each NUMA node (8 /
cluster node).


Single-node execution
---------------------


Distributed execution
---------------------

SLURM
^^^^^

Synchronous
"""""""""""

- salloc


hpx::init: command line warning: --hpx:localities used when running with SLURM, requesting a different number of localities (8) than have been assigned by SLURM (1), the application might not run properly.


Asynchronous
""""""""""""

- sbatch



Example script for starting a LUE framework script with SLURM. This
will execute the model using 96 CPU cores, distributed over 16 processes,
distributed over two nodes in a cluster partition.

.. code-block:: bash

    # Fixed. Depends on platform.
    nr_numa_domains_per_node=8
    nr_cores_per_socket=6
    nr_cpus_per_task=12
    cpu_binding="thread:0-5=core:0-5.pu:0"

    # Depends on size of job
    nr_nodes=2

    # Fixed
    nr_tasks=$(expr $nr_nodes \* $nr_numa_domains_per_node)


    srun \
        --ntasks $nr_tasks \
        --cpus-per-task=$nr_cpus_per_task \
        --cores-per-socket=$nr_cores_per_socket \
        --kill-on-bad-exit \
        python my_model.py \
            --hpx:ini="hpx.parcel.mpi.enable=1" \
            --hpx:ini="hpx.os_threads=$nr_cores_per_socket" \
            --hpx:bind=$cpu_binding \
            --hpx:print-bind


See also: https://hpx-docs.stellar-group.org/latest/html/manual/running_on_batch_systems.html#how-to-use-hpx-applications-with-slurm
