Python
======
.. toctree::
   :hidden:
   :maxdepth: 1
   :caption: Contents:


Example script for executing LUE framework modelling operations on one
or more localities.

.. code-block:: python

    import lue.framework as lfr
    import numpy as np


    # Model logic ------------------------------------------------------------------
    def create_array_2d():
        array_shape = (1000, 1000)
        partition_shape = (100, 100)
        dtype = np.dtype(np.float64)
        fill_value = 5.5

        return lfr.create_array(array_shape, partition_shape, dtype, fill_value)


    def iterate(state):
        print(type(state))
        for t in range(100):
            state = lfr.sqrt(state)
            print(t)

        return state


    def simulate():
        state = iterate(create_array_2d())


    # Initialize HPX runtime and run model, on the root locality -------------------
    # General configuration options, which are valid on all
    # platforms. Platform-specific options can be passed on the command line.
    cfg = [
        # Make sure hpx_main is always executed
        "hpx.run_hpx_main!=1",
        # Allow for unknown command line options
        "hpx.commandline.allow_unknown!=1",
        # Disable HPX' short options
        "hpx.commandline.aliasing!=0",
        # Don't print diagnostics during forced terminate
        "hpx.diagnostics_on_terminate!=0",
        # Make AGAS clean up resources faster than by default
        "hpx.agas.max_pending_refcnt_requests!=50",
    ]

    lfr.start_hpx_runtime(cfg)

    # The root locality will distribute the work over all other
    # localities. Never perform Python code on the other localities than the
    # root locality unless you know what you are doing.
    if lfr.on_root_locality():
        simulate()


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
