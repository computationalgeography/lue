(scalability-quick-start)=

# Quick start

Let us assume we want to perform [scalability experiments](#scalability-experiments) for a very simple model.
This allows us to focus on the experiment itself rather than the model. We will skip over details for now, but
provide links to reference pages with more information.

Performing scalability experiments for a LUE computation involving rasters involves the following steps:

1. Determine a good array partition shape
1. Determine the strong or weak scalability efficiencies

Performing scalability experiments involves a lot of book keeping. A model must be run several times and the
time it stakes to perform all computations has to be saved for post-processing. Information to save includes
the amount of hardware used, the shape of the arrays, the shape of the partitions, and the time it took. To
help with this administration we will use the [lue_scalability.py](#lue-scalability) command[^build].

[^build]: This command should be installed if LUE was [built](#install-source) with the option
`LUE_BUILD_QUALITY_ASSURANCE=TRUE`.

```{note}
This quick start illustrates performing scalability experiments using a LUE model implemented using the Python
language bindings, but everything works very similar when using a model implemented using the C++ API.

For simplicity we will be performing scalability experiments over the CPU cores within a single desktop
computer here, but everything works very similar when performing experiments over cluster nodes in a
multi-node computer cluster.
```


## Model

The model we will be using here is [Conway's Game of
Life](https://en.wikipedia.org/wiki/Conway's_Game_of_Life). A version of the model that is capable of saving
results as rasters is available in the [LUE tutorial
repository](https://github.com/computationalgeography/lue_tutorial). For this quick start, we simplified that
model to be (only) useful in scalability experiments. Here is the complete working model:

```{literalinclude} game_of_life.py
   :caption: game_of_life.py, with the code added for performing scalability experiments highlighted
   :linenos:
   :emphasize-lines: 9, 65, 67, 68, 70, 71, 73, 78, 79, 81, 83, 84
```

A few changes were made to the original model. First an import statement was added for the
`scalability.instrument` sub-package. The code in this sub-package simplifies adhering to the requirements
lue_scalability.py poses on a model.

Next, the part of the model containing the code we want to measure the scalability of was identified. This is
the code between the `run.start()`{l=python} and `run.wait()`{l=python} calls. The time it takes to execute
the statements between those calls ends up in the result file, and is used for further processing. It is
important that time spent on code that should not be part of the measurements has finished executing before
calling `run.start()`{l=python} and that time spent on code that should be part of the measurements has
finished executing before calling `run.end()`{l=python}.

An experiment involves measuring the time it takes to perform one or more model runs. All measurements end up
in the result file. More information about the `scalability.instrument` sub-package can be found in the
[associated reference](#source-lue-qa-python) pages.


## Cluster configuration

The lue_scalability.py command requires information about the cluster used to perform the experiments on. In
this example we will be using a cluster called orkney, which is actually a desktop computer. Its configuration
can be found in the [lue_qa repository](https://github.com/computationalgeography/lue_qa), in
`configuration/orkney/cluster.json`.

```json
{
    "name": "orkney",
    "scheduler": {
        "kind": "shell"
    },
    "nr_cluster_nodes": 1,
    "cluster_node": {
        "nr_packages": 1,
        "package": {
            "nr_numa_nodes": 1,
            "numa_node": {
                "memory": 128,
                "nr_cores": 12,
                "core": {
                    "nr_threads": 2
                }
            }
        }
    }
}
```

The terminology used in describing a cluster is inspired by the [Portable Hardware Locality
(hwloc)](https://www.open-mpi.org/projects/hwloc/) software package.

In case of real multi-node computer clusters, scheduler can be "slurm" and several settings can be added as
well, like the name of the partition to use. The lue_qa repository contains examples for those as well.


## Worker configuration

Since we are using the orkney desktop computer as our cluster here, the worker we are interested in here is
the CPU core. The worker configuration file can be found in the lue_qa repository, in
`configuration/orkney/thread_numa_node/{partition_shape,strong_scalability,weak_scalability}.json`. For each
kind of experiment (partition_shape, strong_scalability, weak_scalability), a worker configuration can be
provided. In the case of the strong and weak scalability experiments it is not unusual for the configurations
to be the same, and a symbolic link can be used to refer to a single file containing this information.

Example configuration for the partition_shape experiment:

```json
{
    "scenario": "thread_numa_node",
    "count": 1,
    "locality_per": "numa_node",
    "worker": {
        "type": "thread",
        "pool": {
            "size": 12
        }
    }
}
```

Example configuration for the strong and weak scalability experiments:

```json
{
    "scenario": "thread_numa_node",
    "count": 1,
    "locality_per": "numa_node",
    "worker": {
        "type": "thread",
        "pool": {
            "min_size": 1,
            "max_size": 12,
            "incrementor": 1
        }
    }
}
```

The count is the number of times an experimental run should be repeated. Doing it more than once allows for
the inspection of the spread in the resulting scalability metrics.


## Experiment configuration


## Partition shape experiment

Given the above configuration files we can start the scalability experiments. The first thing to do is to
determine a good partition shape to use for the strong and weak scalability experiments. For this we can call
lue_scalability.py with the prefixes of the paths containing the cluster, worker, and experiment configuration
files. Additionally, we need to tell it that we want to perform a partition_shape experiment. This results in
a command like this:

```bash
# 100 Is a model-specific argument. In this case the number of generations
# that we want the model to simulate
lue_scalability.py \
    partition_shape script game_of_life.py "100" orkney cpu_core \
    ./lue_qa/configuration ./lue_qa/experiment /tmp/scalability
```

This command will create a Bash script for us, containing all the commands that need to be executed to end up
with enough information to be able to select a good partition shape. The name of the script will be printed.
Its name is based on the information passed in:
`<result_prefix>/<cluster_name>/<worker_name>/<command_name>/<experiment_name>.sh`.

Once the commands from this Bash script have finished executing, all results must be aggregated into a single
LUE dataset called `scalability.lue`, using the same command as before, but using the import stage name
instead of script:

```bash
lue_scalability.py \
    partition_shape import game_of_life.py "100" orkney cpu_core \
    ./lue_qa/configuration ./lue_qa/experiment /tmp/scalability
```

The last step of this experiment involves postprocessing the results. This results in various statistics
ending up in the same LUE dataset as created in the previous step, as well as in a PDF containing a graph
showing the latencies of performing the experimental runs for different partition shapes. For this we can
again use the same command as before, but using the postprocess stage name instead of import:

```bash
lue_scalability.py \
    partition_shape postprocess game_of_life.py "100" orkney cpu_core \
    ./lue_qa/configuration ./lue_qa/experiment /tmp/scalability
```

The experiment is now finished and we can inspect the plot in `plot.pdf` to visually inspect the results. We
are interested in a partition shape for which the latency is the lowest. In general we will have multiple
options that have good (low) latencies.

TODO Show plot


## Strong scalability experiment

Performing a strong scalability experiment involves calling the same commands as in the partition shape
experiment, but now using the strong_scalability experiment name instead of partition_shape:

```bash
lue_scalability.py \
    strong_scalability script game_of_life.py "100" orkney cpu_core \
    ./lue_qa/configuration ./lue_qa/experiment /tmp/scalability
lue_scalability.py \
    strong_scalability import game_of_life.py "100" orkney cpu_core \
    ./lue_qa/configuration ./lue_qa/experiment /tmp/scalability
lue_scalability.py \
    strong_scalability postprocess game_of_life.py "100" orkney cpu_core \
    ./lue_qa/configuration ./lue_qa/experiment /tmp/scalability
```

TODO Show plot


## Weak scalability experiment

Performing a weak scalability experiment involves calling the same commands as in the partition shape and
strong scalability experiment, but now using the weak_scalability experiment name:

```bash
lue_scalability.py \
    weak_scalability script game_of_life.py "100" orkney cpu_core \
    ./lue_qa/configuration ./lue_qa/experiment /tmp/scalability
lue_scalability.py \
    weak_scalability import game_of_life.py "100" orkney cpu_core \
    ./lue_qa/configuration ./lue_qa/experiment /tmp/scalability
lue_scalability.py \
    weak_scalability postprocess game_of_life.py "100" orkney cpu_core \
    ./lue_qa/configuration ./lue_qa/experiment /tmp/scalability
```

TODO Show plot
