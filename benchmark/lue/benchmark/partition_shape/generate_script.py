from ..benchmark import *
from .partition_shape_experiment import *
from ..cluster import *
from .. import job
import os.path


def generate_script_slurm(
        cluster,
        benchmark,
        experiment,
        script_pathname):

    assert benchmark.worker.type == "thread"
    assert experiment.max_duration is not None

    output_filename = job.output_pathname(script_pathname)

    # Iterate over all combinations of array shapes and partition shapes
    # we need to benchmark and format a snippet of bash script for
    # executing the benchmark
    commands = []

    assert False, "Create directories!"

    for array_shape in experiment.array.shapes():
        for partition_shape in experiment.partition.shapes():

            result_pathname = experiment.benchmark_result_pathname(
                cluster.name, array_shape,
                "x".join([str(extent) for extent in partition_shape]), "json")

            commands.append(
                "srun {command_pathname} "
                    '--hpx:ini="hpx.parcel.mpi.enable=1" '
                    '--hpx:ini="hpx.parcel.tcp.enable=0" '
                    '{configuration}'
                .format(
                    command_pathname=experiment.command_pathname,
                    configuration=job.configuration(
                        cluster, benchmark, experiment,
                        array_shape, partition_shape,
                        result_pathname),
                )
            )

    # HPX doc:
    # You can change the number of localities started per node
    # (for example to account for NUMA effects) by specifying the
    # -n option of srun. The number of cores per locality can be
    # set by -c.

    # There is no need to use any of the HPX command line options
    # related to the number of localities, number of threads,
    # or related to networking ports. All of this information is
    # automatically extracted from the SLURM environment by the
    # HPX startup code.

    # The srun documentation explicitly states: "If -c is
    # specified without -n as many tasks will be allocated per node
    # as possible while satisfying the -c restriction. For instance
    # on a cluster with 8 CPUs per node, a job request for 4 nodes
    # and 3 CPUs per task may be allocated 3 or 6 CPUs per node (1
    # or 2 tasks per node) depending upon resource consumption by
    # other jobs." For this reason, we suggest to always specify
    # -n <number-of-instances>, even if <number-of-instances>
    # is equal to one (1).

    with open(script_pathname, "w") as script:
        script.write("""\
#!/usr/bin/env bash
#SBATCH --nodes={nr_nodes}
#SBATCH --ntasks={nr_nodes}
#SBATCH --cpus-per-task={nr_threads}
#SBATCH --output={output_filename}
#SBATCH --partition={partition_name}
#SBATCH --time={max_duration}

set -e

module purge
module load userspace/all
module load gcc/7.2.0
module load boost/gcc72/1.65.1
module load mpich/gcc72/mlnx/3.2.1

{commands}
""".format(
            nr_nodes=benchmark.worker.nr_nodes(),
            nr_threads=benchmark.worker.nr_threads(),
            output_filename=output_filename,
            partition_name=cluster.partition_name,
            max_duration=experiment.max_duration,
            commands="\n".join(commands)
        ))

    print("sbatch {}".format(script_pathname))


def generate_script_shell(
        cluster,
        benchmark,
        experiment,
        script_pathname):

    assert benchmark.worker.type == "thread"

    # Iterate over all combinations of array shapes and partition shapes
    # we need to benchmark and format a snippet of bash script for
    # executing the benchmark
    commands = []

    for array_shape in experiment.array.shapes():
        for partition_shape in experiment.partition.shapes():

            result_pathname = experiment.benchmark_result_pathname(
                cluster.name, array_shape,
                "x".join([str(extent) for extent in partition_shape]), "json")

            commands += [
                # Create directory for the resulting json file
                "mkdir -p {}".format(os.path.dirname(result_pathname)),

                # Run the benchmark, resulting in a json file
                "{command_pathname} "
                    '--hpx:ini="hpx.os_threads={nr_threads}" '
                    '{configuration}'
                    .format(
                        command_pathname=experiment.command_pathname,
                        nr_threads=benchmark.worker.nr_threads(),
                        configuration=job.configuration(
                            cluster, benchmark, experiment,
                            array_shape, partition_shape,
                            result_pathname)
                    )
            ]

    job.write_script(commands, script_pathname)
    print("bash ./{}".format(script_pathname))


def generate_script(
        cluster_settings_json,
        benchmark_settings_json,
        experiment_settings_json,
        script_pathname,
        command_pathname):
    """
    Given a fixed set of workers, iterate over a range of array shapes
    and a range of partition shapes and capture benchmark results

    A shell script is created that submits jobs to the scheduler. Each
    job executes a benchmark and writes results to a JSON file.
    """
    job_scheduler = cluster_settings_json["job_scheduler"]
    assert job_scheduler in ["shell", "slurm"]

    benchmark = Benchmark(benchmark_settings_json)
    assert benchmark.worker.nr_benchmarks() == 1
    assert benchmark.worker.nr_nodes_range() == 0
    assert benchmark.worker.nr_threads_range() == 0

    experiment = PartitionShapeExperiment(
        experiment_settings_json, command_pathname)

    if job_scheduler == "slurm":
        cluster = SlurmCluster(cluster_settings_json)
        generate_script_slurm(
            cluster, benchmark, experiment,
            script_pathname)
    elif job_scheduler == "shell":
        cluster = ShellCluster(cluster_settings_json)
        generate_script_shell(
            cluster, benchmark, experiment,
            script_pathname)
