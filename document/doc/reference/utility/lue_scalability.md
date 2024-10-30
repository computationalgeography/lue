(lue-scalability)=

# lue_scalability.py

The lue_scalability.py command has been created to make it as easy as possible to perform scalability
experiments for LUE computations. Performing scalability experiments involves a lot of book keeping. Given a
set of built-in conventions, performing these experiments become easy to do, on different platforms, like
laptops or computer clusters, and for different computations.

For information about the command's usage, type `lue_scalability.py --help`{l=bash} on the command-line.

There are three kinds of experiments supported by the command, for determining a good array partition shape,
and the strong and weak scalabilities. Each of these is described in more detail below.

Each kind of experiment is split into three stages that have to be performed in order and which results in a
LUE dataset containing the raw latencies of all runs and statistics, and plots. Each of these stages is
described in more detail below.

Here is an example for performing a strong scalability experiment over all CPU cores on a desktop computer
called orkney:

```bash
# Variables used in the commands below
command_name="my_model.py"
command_arguments="--my_parameter=5"
platform_name="orkney"
experiment_name="strong_scalability"
worker_name="core_numa_node"
platform_config_prefix="./platform_configuration/"
experiment_config_prefix="./experiment_configuration/"
result_prefix="./result/"

# Create a Bash script that will start all commands needed for the experiment
lue_scalability.py \
    $experiment_name script $command_name "$command_arguments" \
    $platform_name $worker_name \
    $platform_config_prefix $experiment_config_prefix $result_prefix

# Run the Bash script just created, writing results to a set of JSON files
script="$result_prefix/$platform_name/$command_name/$worker_name/$experiment_name.sh"
bash $script

# Import all experiment results into a single LUE dataset
lue_scalability.py \
    $experiment_name import $command_name "$command_arguments" \
    $platform_name $worker_name \
    $platform_config_prefix $experiment_config_prefix $result_prefix

# Add statistics to the dataset, generate scalability plots
lue_scalability.py \
    $experiment_name postprocess $command_name "$command_arguments" \
    $platform_name $worker_name \
    $platform_config_prefix $experiment_config_prefix $result_prefix
```

Note that, except for the stage (script, import, postprocess), the arguments passed to the lue_scalability.py
command are the same for each invocation.


## Conventions

The conventions used by the lue_scalability.py command are related to:

- Storing information about each *platform*
- Storing information about each *worker*
- Storing information about each *experiment*
- *Calling conventions* for computations


### Platform

To be able to do its work, the command needs to know some properties of the platform, like information about
the scheduler to use to submit commands, the number of nodes in the cluster (partition), the nesting of
hardware resources in a single node, etc.

Properties of a platform are described in a file called `platform.json`. Examples of platform configurations for
several platforms used by the LUE R&D team can be found in the [lue_qa
repository](https://github.com/computationalgeography/lue_qa). In case you are using one of those platforms,
you can clone the project and pass the pathname of the configuration directory as an argument to the
lue_scalability.py command. Otherwise, these configuration can be used as inspiration for describing other
platforms. Platform configuration files must be named after the platform:
`<platform_config_prefix>/<platform_name>.json`.

Once setup correctly, platform configuration files likely won't have to be changed anymore.


### Workers

A worker is an amount of hardware which will be increased during a scalability experiment. Supported
kinds of hardware are CPU cores, NUMA nodes, and cluster nodes. The output of the strong and weak scalability
experiments are *relative* scalability measures, which means that they are relative to the latency of the
software when using a single worker (CPU core, NUMA node, or cluster node).

Like platforms, workers have to be described as well. Examples of configuration files can be found in the above
mentioned lue_qa repository. The worker configuration files are named after the worker and the experiment, and
must be nested in the platform configuration file directories:
`<platform_config_prefix>/<platform_name>/<worker_name>/<experiment_name>.json`.

Once setup correctly, worker configuration files likely won't have to be changed anymore.


### Experiments

Each experiment (partition shape, strong or weak scalability) needs to be described in a configuration file as
well. In this file, properties are described which are unique to the command tested. For example, depending on
the computation, larger or smaller arrays or partition sizes need to be used.

Examples of configuration files can be found in the above mentioned lue_qa repository, in the `experiment`
directory. The experiment configuration files need to be put in a directory named after the platform name and
experiment name, and the name of the command tested, including any file extension this command might have:
`<experiment_config_prefix>/<platform_name>/<command_name>/<worker_name>/<experiment_name>.json`

Contrary to the platform and worker configuration files, configuration files for experiments need to be created
for each command tested.


### Calling conventions

The lue_scalability.py command requires each command to be tested to support certain command line options.
These are used to vary those aspects of the experimental run that are required to be able to compute
scalability measures afterwards. Two sets of command line options are used for this. The first ones, prefixed
by `hpx:` are targeted at the HPX runtime used by LUE. Each LUE computation automatically supports these
options so there is no need to make any code changes for this. The second set of options is prefixed by `lue:`
and is specific for scalability experiments. The next table lists all options:

| Option                                    | Effect                                                |
| ---                                       | ---                                                   |
| `--lue:count=<count>`                     | Number of times the computations need to be performed |
| `--lue:nr_workers=<nr_workers>`           | Number of workers used to perform the computations    |
| `--lue:array_shape=<array_shape>`         | Shape of arrays to use                                |
| `--lue:partition_shape=<partition_shape>` | Shape of partitions to use                            |
| `--lue:result=<result>`                   | Pathname of JSON file to store results in             |

The author of the command used in scalability experiments is responsible for make sure the command behaves
correctly to the arguments to these options passed in.

The count is used for statistical purposes. Performing computations multiple times and recording latencies for
each of those runs allows for the analysis of spread in the results.

The number of workers is a value to store in the result file. It is unlikely that this value has to be used by
the computation itself.

The result pathname is the name of the file in which the results of the experimental runs need to be stored.

LUE contains support code which can be used to instrument code for measuring and storing results of
experimental runs. This makes it easy to save the necessary information to files without having to know about
file formats that have to be used. Here is an example of how this works for a Python script that uses LUE.


```python
import lue.framework as lfr
import lue.qa.scalability.instrument as lqi


def run_my_model() -> None:
    pass


@lfr.runtime_scope
def my_model(
    count: int,
    nr_workers: int,
    array_shape: tuple[int, int],
    partition_shape: tuple[int, int],
    result_pathname: str,
) -> None:
    # Before calling start(), wait for any irrelevant code (I/O?) to finish. Otherwise the time this code
    # takes to execute will be part of the measurement.
    # Before calling stop(), wait for any relevant code (model!) to finish. Otherwise the time this code
    # takes will not be completely part of the measurement.

    experiment = lqi.ArrayExperiment(nr_workers, array_shape, partition_shape)
    experiment.start()

    for _ in range(count):
        run = lqi.Run()

        run.start()
        run_my_model()
        run.stop()
        experiment.add(run)

    experiment.stop()
    lqi.save_results(experiment, result_pathname)
```


A similar approach can be taken for C++ code using LUE. For more information about this, see the [reference
pages about LUE quality assurance](#source-lue-qa).


## Experiments

See the [scalability quick start](#scalability-quick-start) for outputs of the experiment for an example
model.


### Partition shape

Before doing strong and weak scalability experiments, a good partition shape must be determined. Using the
wrong partition shape results in bad results. In short:

- Small partitions → many tasks → much overhead → higher latencies
- Large partitions → few tasks → not enough work to keep all workers busy → higher latencies

What entails a good partition shape depends on various factors, like the specific operations used in the
computation and the properties of the hardware. LUE has built-in support for determining default partition
sizes, which is convenient, but these may not (yet) result in the best performance and scalability.

The goal of the partition shape experiment is to identify the range of good partition shapes to use. The
experiment involves trying out different partition shapes, and recording the associated latencies of the
computations. In the experiment configuration file, the user can define the smallest and largest partition
shapes to test, and which shapes to test in between those extremes. When the range of partition shapes to test
is configured correctly (this may involve some trial and error), the resulting plot showing the latency by
partition shape will show a range of good partition shapes to use.


### Strong scalability

The goal of a strong scalability experiment is to determine how well software is capable to use additional
workers to decrease the latency of the computations. For this, a certain total problem size needs to be
configured, as well a range of workers to try. After the latencies have been recorded, [strong scalability
efficiencies](#strong-scalability-experiment) can be calculated and plotted.


### Weak scalability

The goal of a weak scalability experiment is to determine how well software is capable to use additional
workers to process a larger number of computations. For this, a certain problem size per worker needs to be
configured, as well a range of workers to try. After the latencies have been recorded, [weak scalability
efficiencies](#weak-scalability-experiment) can be calculated and plotted.


## Stages

All scalability experiments supported by the command are divided into three stages -- script, import, and
postprocess -- which must be run in that order.


### Script

In the script stage the command outputs a Bash script that contains all commands needed to end up with all the
information required for the subsequent stages. Depending on the scheduler, this script will synchronously
execute the commands in order, or it will pass the commands to a job-scheduler, to be asynchronously
scheduled once the required hardware resources have become available.

```{note}
In case the commands in the script are passed on to a scheduler for asynchronous execution, continuing with
the subsequent stages must be postponed until all commands have finished writing their result files.
```

As a side-effect of this stage, a LUE dataset called `raw.lue` is written containing all information about the
experiment performed, including the Bash script itself.

This is the directory and file layout after running this stage for a partition shape experiment:

```bash
orkney/
└── core_numa_node
    └── game_of_life.py
        ├── partition_shape
        │   └── raw.lue
        └── partition_shape.sh
```

When running this stage multiple times for the same configuration, the command will rename the `<experiment>`
directory (partition_shape, strong_scalability, or weak_scalability) by appending the last modification date
to the directory name. This way existing results will not be overwritten.

After running the Bash script created by this stage for a partition shape experiment, the directory and file
layout looks like this:

```bash
orkney/
└── core_numa_node
    └── game_of_life.py
        ├── partition_shape
        │   ├── 10000x10000
        │   │   ├── 1000x1000.json
        │   │   ├── 1060x1060.json
        │   │   ├── 1118x1118.json
        │   │   ├── 500x500.json
        │   │   ├── 612x612.json
        │   │   ├── 707x707.json
        │   │   ├── 790x790.json
        │   │   ├── 866x866.json
        │   │   └── 935x935.json
        │   └── raw.lue
        └── partition_shape.sh
```


### Import

Once all commands from the script stage have finished executing, the import stage will aggregate the
information from `raw.lue` and all individual output files written by the commands in the Bash script into a
single LUE data set called `scalability.lue`. This file contains all information related to the scalability
experiment, in an open data format. It can be used as a backup of the results for the long term.

After running this stage for a partition shape experiment, the above directory and file layout now looks like
this:

```bash
orkney/
└── core_numa_node
    └── game_of_life.py
        ├── partition_shape
        │   ├── 10000x10000
        │   │   ├── 1000x1000.json
        │   │   ├── 1060x1060.json
        │   │   ├── 1118x1118.json
        │   │   ├── 500x500.json
        │   │   ├── 612x612.json
        │   │   ├── 707x707.json
        │   │   ├── 790x790.json
        │   │   ├── 866x866.json
        │   │   └── 935x935.json
        │   ├── raw.lue
        │   └── scalability.lue
        └── partition_shape.sh
```

Each JSON file contains results for running the experiment for a certain array shape and partition shape.


### Postprocess

After the import stage has finished successfully, the postprocess stage computes statistics and creates a plot
(`plot.pdf`) containing one or more graphs illustrating the results of the scalability experiments.

After running this stage for a partition shape experiment, the above directory and file layout now looks like
this:

```bash
orkney/
└── core_numa_node
    └── game_of_life.py
        ├── partition_shape
        │   ├── 10000x10000
        │   │   ├── 1000x1000.json
        │   │   ├── 1060x1060.json
        │   │   ├── 1118x1118.json
        │   │   ├── 500x500.json
        │   │   ├── 612x612.json
        │   │   ├── 707x707.json
        │   │   ├── 790x790.json
        │   │   ├── 866x866.json
        │   │   ├── 935x935.json
        │   │   └── plot.pdf
        │   ├── graph.pdf
        │   ├── plot.pdf
        │   ├── raw.lue
        │   └── scalability.lue
        └── partition_shape.sh
```

In case custom postprocessing needs to happen, for example for creating tables or plots for a publication,
additional scripts can be created that read the relevant information directly from `scalability.lue`. Each LUE
dataset is an HDF5 file, so besides using the LUE data model Python package, any software that is capable of
reading HDF5 files can be used for this, like [h5py](https://www.h5py.org/) for example. A graph of the layout
of the LUE data set is written in the postprocess stage as well; see `graph.pdf`. The HDFView application
released by the [HDF Group](https://www.hdfgroup.org/) can also be used to browse the layout and contents of
LUE datasets.
