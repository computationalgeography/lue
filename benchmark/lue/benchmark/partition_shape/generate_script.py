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

    # Iterate over all combinations of array shapes and partition shapes
    # we need to benchmark and format a snippet of bash script for
    # executing the benchmark
    job_steps = []

    for array_shape in experiment.array.shapes():
        for partition_shape in experiment.partition.shapes():

            result_pathname = experiment.benchmark_result_pathname(
                cluster.name, array_shape,
                "x".join([str(extent) for extent in partition_shape]), "json")

            job_steps += [
                # Create directory for the resulting json file. This
                # only needs to run on one of the nodes. For this we
                # create a sub-allocation of one node and one task.
                "srun --nodes 1 --ntasks 1 mkdir -p {}"
                    .format(os.path.dirname(result_pathname)),

                # Run the benchmark, resulting in a json file
                "srun {srun_configuration} {command_pathname} "
                        '--hpx:ini="hpx.parcel.mpi.enable=1" '
                        '{program_configuration}'
                    .format(
                        srun_configuration=job.srun_configuration(),
                        command_pathname=experiment.command_pathname,
                        program_configuration=job.program_configuration(
                            cluster, benchmark, experiment,
                            array_shape, partition_shape,
                            result_pathname),
                    )
            ]

    slurm_script = job.create_slurm_script(
        nr_nodes=benchmark.worker.nr_nodes(),
        nr_threads=benchmark.worker.nr_threads(),
        output_filename=experiment.result_pathname(
            cluster.name,
            os.path.basename(os.path.splitext(script_pathname)[0]), "out"),
        partition_name=cluster.partition_name,
        max_duration=experiment.max_duration,
        job_steps=job_steps)

    job_name = "{name}-{program_name}".format(
        name=experiment.name,
        program_name=experiment.program_name)
    delimiter = "END_OF_SLURM_SCRIPT"

    commands = [
        "# Make sure SLURM can create the output file",
        "mkdir -p {}".format(experiment.workspace_pathname(cluster.name)),
        "",
        "# Submit job to SLURM scheduler",
        "sbatch --job-name {job_name} << {delimiter}".format(
            job_name=job_name, delimiter=delimiter),
        slurm_script,
        "{delimiter}".format(delimiter=delimiter),
    ]

    job.write_script(commands, script_pathname)
    print("bash ./{}".format(script_pathname))


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
                    '{program_configuration}'
                    .format(
                        command_pathname=experiment.command_pathname,
                        nr_threads=benchmark.worker.nr_threads(),
                        program_configuration=job.program_configuration(
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

    if job_scheduler == "slurm":
        cluster = SlurmCluster(cluster_settings_json)
    elif job_scheduler == "shell":
        cluster = ShellCluster(cluster_settings_json)

    benchmark = Benchmark(benchmark_settings_json, cluster)
    assert benchmark.worker.nr_benchmarks() == 1
    assert benchmark.worker.nr_nodes_range() == 0
    assert benchmark.worker.nr_threads_range() == 0

    experiment = PartitionShapeExperiment(
        experiment_settings_json, command_pathname)

    if job_scheduler == "slurm":
        generate_script_slurm(cluster, benchmark, experiment, script_pathname)
    elif job_scheduler == "shell":
        generate_script_shell(cluster, benchmark, experiment, script_pathname)
