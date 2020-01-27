from ..benchmark import *
from .strong_scaling_experiment import *
from ..cluster import *
from .. import job
import os.path


def generate_script_slurm_threads(
        cluster,
        benchmark,
        experiment,
        script_pathname):
    """
    Scale over threads in a single cluster node
    """
    assert benchmark.worker.nr_nodes_range() == 0
    assert benchmark.worker.nr_threads_range() >= 1

    job_steps = []
    array_shape = experiment.array.shape()
    partition_shape = experiment.partition.shape()

    for benchmark_idx in range(benchmark.worker.nr_benchmarks()):

        nr_workers = benchmark.worker.nr_workers(benchmark_idx)

        result_pathname = experiment.benchmark_result_pathname(
            cluster.name, benchmark.scenario_name, nr_workers, "json")

        job_steps += [
            # Run the benchmark, resulting in a json file
            "srun {srun_configuration} {command_pathname} "
                '--hpx:ini="hpx.parcel.mpi.enable=1" '
                '--hpx:ini="hpx.os_threads={nr_threads}" '
                '{program_configuration}'
                .format(
                    srun_configuration=job.srun_configuration(cluster),
                    command_pathname=experiment.command_pathname,
                    nr_threads=nr_workers,
                    program_configuration=job.program_configuration(
                        cluster, benchmark, experiment,
                        array_shape, partition_shape,
                        nr_workers=nr_workers),
                )
        ]

    slurm_script = job.create_slurm_script(
        nr_nodes=benchmark.worker.nr_nodes(),
        nr_threads=cluster.node.nr_threads(),  # Reserve all threads
        output_filename=experiment.result_pathname(
            cluster.name, benchmark.scenario_name,
            os.path.basename(os.path.splitext(script_pathname)[0]), "out"),
        partition_name=cluster.scheduler.settings.partition_name,
        max_duration=experiment.max_duration,
        job_steps=job_steps)

    job_name = "{name}-{program_name}".format(
        name=experiment.name,
        program_name=experiment.program_name)
    delimiter = "END_OF_SLURM_SCRIPT"

    commands = [
        "# Make sure SLURM can create the output file",
        "mkdir -p {}".format(experiment.workspace_pathname(
            cluster.name, benchmark.scenario_name)),
        "",
        "# Submit job to SLURM scheduler",
        "sbatch --job-name {job_name} << {delimiter}".format(
            job_name=job_name, delimiter=delimiter),
        slurm_script,
        "{delimiter}".format(delimiter=delimiter),
    ]

    job.write_script(commands, script_pathname)
    print("bash ./{}".format(script_pathname))


def generate_script_slurm_nodes(
        cluster,
        benchmark,
        experiment,
        script_pathname):
    """
    Scale over nodes in a cluster of nodes
    """
    assert benchmark.worker.nr_nodes_range() >= 1
    assert benchmark.worker.nr_threads_range() == 0

    # Given a problem size, measure the duration of executing an
    # executable on increasingly more nodes. For each number of nodes
    # to use, we create a SLURM script containing the job steps for that
    # partition. Then we create a shell script that executes the SLURM
    # scripts, passing jobs to the scheduler.

    # Iterate over all sets of workers we need to benchmark and format
    # a snippet of bash script for executing the benchmark

    commands = []
    array_shape = experiment.array.shape()
    partition_shape = experiment.partition.shape()

    # for nr_workers in \
    #         range(benchmark.worker.min_nr, benchmark.worker.max_nr + 1):
    for benchmark_idx in range(benchmark.worker.nr_benchmarks()):

        nr_workers = benchmark.worker.nr_workers(benchmark_idx)

        result_pathname = experiment.benchmark_result_pathname(
            cluster.name, benchmark.scenario_name, nr_workers, "json")

        job_steps = [
            # Run the benchmark, resulting in a json file
            "srun {srun_configuration} {command_pathname} "
                '--hpx:ini="hpx.parcel.mpi.enable=1" '
                '--hpx:ini="hpx.os_threads={nr_threads}" '
                '{program_configuration}'
                .format(
                    srun_configuration=job.srun_configuration(cluster),
                    command_pathname=experiment.command_pathname,
                    nr_threads=cluster.node.nr_cores(),  # One thread per core
                    program_configuration=job.program_configuration(
                        cluster, benchmark, experiment,
                        array_shape, partition_shape,
                        nr_workers=nr_workers),
                )
        ]

        slurm_script = job.create_slurm_script(
            nr_nodes=nr_workers,
            nr_threads=benchmark.worker.nr_threads(),
            output_filename=experiment.benchmark_result_pathname(
                cluster.name, benchmark.scenario_name, nr_workers, "out"),
            partition_name=cluster.scheduler.settings.partition_name,
            max_duration=experiment.max_duration,
            job_steps=job_steps)

        job_name = "{name}-{program_name}-{nr_workers}".format(
            name=experiment.name,
            program_name=experiment.program_name,
            nr_workers=nr_workers)
        delimiter = "END_OF_SLURM_SCRIPT_{}".format(nr_workers)

        commands += [
            # Create a snippet of bash script that creates a SLURM script
            # for this partition. Note that this has to be done at runtime
            # because there is no directory structure yet for storing
            # SLURM scripts.

            "# Number of nodes: {}".format(nr_workers),

            # Create directory for storing SLURM script and benchmark
            # results
            "mkdir -p {}".format(os.path.dirname(result_pathname)),

            # Submit SLURM script to scheduler
            "sbatch --job-name {job_name} << {delimiter}".format(
                job_name=job_name, delimiter=delimiter),
            slurm_script,
            "{delimiter}".format(delimiter=delimiter),
            "",
        ]

    job.write_script(commands, script_pathname)
    print("bash ./{}".format(script_pathname))


def generate_script_slurm(
        cluster,
        benchmark,
        experiment,
        script_pathname):

    assert benchmark.worker.type in ["node", "thread"]

    if benchmark.worker.nr_nodes_range() == 0:
        generate_script_slurm_threads(
            cluster, benchmark, experiment, script_pathname)
    else:
        generate_script_slurm_nodes(
            cluster, benchmark, experiment, script_pathname)


def generate_script_shell(
        cluster,
        benchmark,
        experiment,
        script_pathname):

    assert benchmark.worker.type == "thread"
    assert benchmark.worker.nr_nodes_range() == 0
    assert benchmark.worker.nr_threads_range() >= 1

    # Iterate over all sets of workers we need to benchmark and format
    # a snippet of bash script for executing the benchmark
    commands = []

    array_shape = experiment.array.shape()
    partition_shape = experiment.partition.shape()

    # for nr_workers in \
    #         range(benchmark.worker.min_nr, benchmark.worker.max_nr + 1):
    for benchmark_idx in range(benchmark.worker.nr_benchmarks()):

        nr_workers = benchmark.worker.nr_workers(benchmark_idx)

        result_pathname = experiment.benchmark_result_pathname(
            cluster.name, benchmark.scenario_name, nr_workers, "json")

        commands += [
            # Create directory for the resulting json file
            "mkdir -p {}".format(os.path.dirname(result_pathname)),

            # Run the benchmark, resulting in a json file
            "{command_pathname} "
                '--hpx:ini="hpx.os_threads={nr_threads}" '
                '{program_configuration}'
                .format(
                    command_pathname=experiment.command_pathname,
                    nr_threads=nr_workers,
                    program_configuration=job.program_configuration(
                        cluster, benchmark, experiment,
                        array_shape, partition_shape,
                        nr_workers=nr_workers)
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
    Given a fixed array size and partition shape size, iterate over a
    range of sets of workers and capture benchmark results

    A shell script is created that submits jobs to the scheduler. Each
    job executes a benchmark and writes results to a JSON file.
    """
    cluster = Cluster(cluster_settings_json)

    benchmark = Benchmark(benchmark_settings_json, cluster)
    assert \
        (benchmark.worker.nr_nodes_range() > 0 and not
            benchmark.worker.nr_threads_range() > 0) or \
        (not benchmark.worker.nr_nodes_range() > 0 and  # xor
            benchmark.worker.nr_threads_range() > 0)
    assert benchmark.worker.nr_benchmarks() > 0

    experiment = StrongScalingExperiment(
        experiment_settings_json, command_pathname)

    if cluster.scheduler.kind == "slurm":
        generate_script_slurm(cluster, benchmark, experiment, script_pathname)
    elif cluster.scheduler.kind == "shell":
        generate_script_shell(cluster, benchmark, experiment, script_pathname)
