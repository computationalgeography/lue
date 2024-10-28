(lue-scalability)=

# lue_scalability.py

The lue_scalability.py command has been created to make it as easy as possible to perform scalability
experiments for LUE computations. Performing scalability experiments involves a lot of book keeping. Given a
set of built-in conventions, performing these experiments become easy to do, on different platforms, like
laptops and computer clusters, and for different computations.

There are three kinds of experiments supported by the command: `partition_shape`, `strong_scalability`, and
`weak_scalability`. Each of these will be described in more detail below.

Each kind of experiment is split into three stages: `script`, `import`, and `post_process`. Each of these
stages will be described in more detail below.

Here is an example for performing a strong scalability experiment over all CPU cores on a desktop computer
called "orkney":

```bash
# Variables used in the commands below
command_name="my_model"
command_arguments="--my_parameter=5"
cluster_name="orkney"
experiment_name="strong_scalability"
worker_name="cpu_core"
cluster_config_prefix="./cluster_configuration/"
experiment_config_prefix="./experiment_configuration/"
result_prefix="./result/"

# Create a Bash script that will start all commands needed for the experiment
lue_scalability.py \
    $experiment_name script $command_name "$command_arguments" \
    $cluster_name $worker_name $experiment_name \
    $cluster_config_prefix $experiment_config_prefix $result_prefix

# Run the Bash script just created, writing results to a set of JSON files
script="$result_prefix/$cluster_name/$worker_name/$command_name/$experiment_name.sh"
bash $script

# Import all experiment results into a single LUE dataset
lue_scalability.py \
    $experiment_name import $command_name "$command_arguments" \
    $cluster_name $worker_name $experiment_name \
    $cluster_config_prefix $experiment_config_prefix $result_prefix

# Add statistics to the dataset, generate scalability plots
lue_scalability.py \
    $experiment_name postprocess $command_name "$command_arguments" \
    $cluster_name $worker_name $experiment_name \
    $cluster_config_prefix $experiment_config_prefix $result_prefix
```

The conventions used by the lue_scalability.py command are related to:

- Storing information about each *cluster*: set of files describing characteristics of the cluster
- Storing information about each *worker*: set of files describing characteristics of the worker
- Storing information about each *experiment*: set of files describing characteristics of the experiment
- *Calling conventions* for computations: command-line argument syntax to use


## Conventions

### Cluster

To be able to do its work, the command needs to know some properties of the cluster, like information about
the scheduler to use to submit commands, the number of nodes in the cluster (partition), the nesting of
hardware resources in a single node, etc.

The command treats each platform as a cluster, so the terminology used to describe the properties of a laptop,
for example, is the same as that used to describe a real computer cluster.

Properties of a cluster are described in a file called `cluster.json`. Examples of cluster configurations for
several platforms used by the LUE R&D team can be found in the [lue_qa
repository](https://github.com/computationalgeography/lue_qa). In case you are using one of those clusters,
you can clone the project and pass the location as an argument to the lue_scalability.py command. Otherwise,
they can be used as inspiration for describing other clusters. Cluster configuration files must be named after
the cluster: `<cluster_config_prefix>/<cluster_name.json`.

Once setup, cluster configuration files likely won't change anymore when performing scalability experiments.


### Workers

A worker is an amount of hardware which will be increased linearly during a scalability experiment. Supported
kinds of hardware are CPU cores, NUMA nodes, and cluster nodes. The output of the strong and weak scalability
experiments are *relative* scalability measures, which means that they are relative to the latency of the
software when using a single worker (CPU core, NUMA node, or cluster node).

Like clusters, workers have to be described as well, to enable the lue_scalability.py command to understand
how many workers to create commands for and how to increase the number of workers for different experimental
runs. For example, it is possible to increase the number of workers by one, or some other amount, and to limit
the maximum number of workers to use.

Examples of configuration files can be found in the above mentioned lue_qa repository. The worker
configuration files are named after the worker and experiment, and nested in the cluster configuration file
directories: `<cluster_config_prefix>/<cluster_name>/<worker_name>/<experiment_name>.json`.

Once setup, worker configuration files likely won't change anymore when performing scalability experiments.


### Experiments

Each experiment needs to be described in a configuration file as well. In this file, properties are described
which are unique to the experiment. For example, depending on the computation larger or smaller arrays or
partition sizes need to be used.

Examples of configuration files can be found in the above mentioned lue_qa repository, in the `experiment`
directory. The experiment configuration files need to be put in a directory named after the cluster name and
experiment name, and the name of the command tested, including any file extension this command might have:
`<experiment_config_prefix>/<cluster_name>/<worker_name>/<command_name>/<experiment_name>.json`

Contrary to the cluster and worker configuration files, configuration files for experiments need to be created
for each command tested.


### Calling conventions

The lue_scalability.py command requires each command to be tested to support certain command line options.
These are used to vary those aspects of the experimental run that are required to be able to compute
scalability measures afterwards. Two sets of command line options are used for this. The first ones, prefixed
by `hpx:` are targeted at the HPX runtime used by LUE. Each LUE computation automatically supports these
options so there is no need to make any changes for this. The second set of options is prefixed by `lue:` and
is specific for scalability experiments. The next table lists all options:

| Option                                    | Effect                                                |
| ---                                       | ---                                                   |
| `--lue:count=<count>`                     | Number of times the computations need to be performed |
| `--lue:nr_workers=<nr_workers>`           | Number of workers used to perform the computations    |
| `--lue:array_shape=<array_shape>`         | Shape of arrays to use                                |
| `--lue:partition_shape=<partition_shape>` | Shape of partitions to use                            |
| `--lue:result=<result>`                   | Pathname of JSON file to store results in             |

The author of the command used in scalability experiments is responsible for make sure the command behaves
accordingly to the arguments to these options passed in.

The count is used for statistical purposes. Performing computations multiple times and recording latencies for
each of those runs allows for the analysis of spread in the results.

The number of workers is a value to store in the result file. It is unlikely that this value has to be used by
the computation itself.

The result pathname is the name of the file in which the results of the experimental runs need to be stored.

LUE contains support code which can be used to instrument code for measuring and storing results of
experimental runs. This is an example of how this works for a Python script that used LUE.


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
    # Before calling start(), wait for any unrelevant code (I/O?) to finish. Otherwise the time this code
    # takes to execute will be part of the measurement.
    # Before calling stop(), wait for any relevant code (model?) to finish. Otherwise the time this code
    # takes will not be part of the measurement.

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

### `partition_shape`


### `strong_scalability`


### `weak_scalability`




## Stages

### `script`


### `import`


### `post_process`
