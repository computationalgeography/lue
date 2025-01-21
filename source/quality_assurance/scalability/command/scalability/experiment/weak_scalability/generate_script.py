import os.path
from functools import reduce  # Python 3

from .. import dataset, job
from .configuration import Configuration


# Scaling the amount of work per number of workers can be done
# by multiplying array_shape_per_worker by some factor. This factor
# depends on the rank. If rank is 1, extents can be scaled by
# nr_workers * 1. In higher dimensional cases, the work scale factor
# lies between 1 and nr_workers.
def work_scale_multiplier(rank, nr_workers):
    return nr_workers ** (1.0 / rank)


def scale_array_shape(array_shape_per_worker, nr_workers):
    rank = len(array_shape_per_worker)
    work_size_per_worker = reduce((lambda x, y: x * y), array_shape_per_worker)
    multiplier = work_scale_multiplier(rank, nr_workers)
    assert 1 <= multiplier <= nr_workers, multiplier

    array_shape = tuple(int(round(multiplier * e)) for e in array_shape_per_worker)

    # We have rounded to whole numbers, but the result should be OK
    work_size = reduce((lambda x, y: x * y), array_shape)
    work_size_we_want = nr_workers * work_size_per_worker
    delta = abs(work_size - work_size_we_want) / (work_size_we_want)
    assert delta < 1e-3, delta  # Accept <= 0.1% difference

    return array_shape


def generate_script_slurm_threads(
    result_prefix, cluster, benchmark, experiment, script_pathname
):
    """
    Scale over threads in a single node
    """
    assert benchmark.worker.nr_cluster_nodes_range == 0
    assert benchmark.worker.nr_numa_nodes_range == 0
    assert benchmark.worker.nr_threads_range >= 1

    job_steps = []
    array_shape_per_worker = experiment.array.shape
    partition_shape = experiment.partition.shape

    for benchmark_idx in range(benchmark.worker.nr_benchmarks):
        nr_workers = benchmark.worker.nr_workers(benchmark_idx)
        nr_threads = nr_workers
        array_shape = scale_array_shape(array_shape_per_worker, nr_workers)
        result_pathname = experiment.benchmark_result_pathname(
            result_prefix, cluster.name, benchmark.scenario_name, nr_workers, "json"
        )

        job_steps += [
            # Run the benchmark, resulting in a json file
            f"mpirun --n {nr_tasks} {mpirun_configuration} {experiment.command_pathname} {experiment.command_arguments} "
            # '--hpx:ini="hpx.parcel.mpi.enable=1" '
            # '--hpx:ini="hpx.os_threads={nr_threads}" '
            '--hpx:threads="{nr_threads}" '
            # '--hpx:bind="{thread_binding}" '
            "{program_configuration}".format(
                nr_tasks=1,  # Single NUMA node or cluster node
                mpirun_configuration=job.mpirun_configuration(cluster),
                # srun_configuration=job.srun_configuration(cluster),
                nr_threads=nr_threads,
                # thread_binding=util.thread_binding(nr_threads),
                program_configuration=job.program_configuration(
                    result_prefix,
                    cluster,
                    benchmark,
                    experiment,
                    array_shape,
                    partition_shape if nr_workers > 1 else array_shape,
                    nr_workers=nr_workers,
                ),
            )
        ]

    slurm_script = job.create_slurm_script(
        cluster,
        nr_cluster_nodes=benchmark.worker.nr_cluster_nodes,
        nr_tasks=cluster.nr_localities_to_reserve(
            benchmark.worker, benchmark.locality_per
        ),
        nr_cores_per_socket=cluster.cluster_node.package.numa_node.nr_cores,
        cpus_per_task=benchmark.nr_logical_cores_per_locality,
        ### # nr_cores_per_numa_node=cluster.cluster_node.package.numa_node.nr_cores,
        ### nr_threads=cluster.cluster_node.nr_threads,
        output_filename=experiment.result_pathname(
            result_prefix,
            cluster.name,
            benchmark.scenario_name,
            os.path.basename(os.path.splitext(script_pathname)[0]),
            "out",
        ),
        partition_name=cluster.scheduler.settings.partition_name,
        sbatch_options=cluster.scheduler.settings.sbatch_options,
        max_duration=experiment.max_duration,
        job_steps=job_steps,
    )

    job_name = "{name}-{program_name}".format(
        name=experiment.name, program_name=experiment.program_name
    )
    delimiter = "END_OF_SLURM_SCRIPT"

    commands = [
        "# Make sure SLURM can create the output file",
        "mkdir -p {}".format(
            experiment.workspace_pathname(
                result_prefix, cluster.name, benchmark.scenario_name
            )
        ),
        "",
        "# Submit job to SLURM scheduler",
        "sbatch --job-name {job_name} {sbatch_options} << {delimiter}".format(
            job_name=job_name,
            sbatch_options=" ".join(cluster.scheduler.settings.sbatch_options),
            delimiter=delimiter,
        ),
        slurm_script,
        "{delimiter}".format(delimiter=delimiter),
    ]

    job.write_script(commands, script_pathname)
    print("bash {}".format(script_pathname))


def generate_script_slurm_numa_nodes(
    result_prefix, cluster, benchmark, experiment, script_pathname
):
    """
    Scale over NUMA nodes in a single cluster node
    """
    assert benchmark.worker.nr_cluster_nodes_range == 0
    assert benchmark.worker.nr_numa_nodes_range >= 1
    assert benchmark.worker.nr_threads_range == 0

    job_steps = []
    array_shape_per_worker = experiment.array.shape
    partition_shape = experiment.partition.shape

    nr_threads = benchmark.worker.nr_threads

    for benchmark_idx in range(benchmark.worker.nr_benchmarks):
        nr_workers = benchmark.worker.nr_workers(benchmark_idx)
        nr_localities = nr_workers
        array_shape = scale_array_shape(array_shape_per_worker, nr_workers)
        result_pathname = experiment.benchmark_result_pathname(
            result_prefix, cluster.name, benchmark.scenario_name, nr_workers, "json"
        )

        job_steps += [
            # Run the benchmark, resulting in a json file
            f"mpirun --n {nr_tasks} {mpirun_configuration} {experiment.command_pathname} {experiment.command_arguments} "
            # '--hpx:ini="hpx.parcel.mpi.enable=1" '
            # '--hpx:ini="hpx.os_threads={nr_threads}" '
            '--hpx:threads="{nr_threads}" '
            # '--hpx:bind="{thread_binding}" '
            "{program_configuration}".format(
                nr_tasks=nr_localities,
                mpirun_configuration=job.mpirun_configuration(cluster),
                # srun_configuration=job.srun_configuration(cluster),
                nr_threads=nr_threads,
                # thread_binding=util.thread_binding(nr_threads),
                program_configuration=job.program_configuration(
                    result_prefix,
                    cluster,
                    benchmark,
                    experiment,
                    array_shape,
                    partition_shape,
                    nr_workers=nr_workers,
                ),
            )
        ]

    slurm_script = job.create_slurm_script(
        cluster,
        nr_cluster_nodes=benchmark.worker.nr_cluster_nodes,
        nr_tasks=cluster.nr_localities_to_reserve(
            benchmark.worker, benchmark.locality_per
        ),
        nr_cores_per_socket=cluster.cluster_node.package.numa_node.nr_cores,
        cpus_per_task=benchmark.nr_logical_cores_per_locality,
        # # nr_cores_per_numa_node=cluster.cluster_node.package.numa_node.nr_cores,
        # nr_threads=cluster.cluster_node.nr_threads,
        output_filename=experiment.result_pathname(
            result_prefix,
            cluster.name,
            benchmark.scenario_name,
            os.path.basename(os.path.splitext(script_pathname)[0]),
            "out",
        ),
        partition_name=cluster.scheduler.settings.partition_name,
        sbatch_options=cluster.scheduler.settings.sbatch_options,
        max_duration=experiment.max_duration,
        job_steps=job_steps,
    )

    job_name = "{name}-{program_name}".format(
        name=experiment.name, program_name=experiment.program_name
    )
    delimiter = "END_OF_SLURM_SCRIPT"

    commands = [
        "# Make sure SLURM can create the output file",
        "mkdir -p {}".format(
            experiment.workspace_pathname(
                result_prefix, cluster.name, benchmark.scenario_name
            )
        ),
        "",
        "# Submit job to SLURM scheduler",
        "sbatch --job-name {job_name} {sbatch_options} << {delimiter}".format(
            job_name=job_name,
            sbatch_options=" ".join(cluster.scheduler.settings.sbatch_options),
            delimiter=delimiter,
        ),
        slurm_script,
        "{delimiter}".format(delimiter=delimiter),
    ]

    job.write_script(commands, script_pathname)
    print("bash {}".format(script_pathname))


def generate_script_slurm_cluster_nodes(
    result_prefix, cluster, benchmark, experiment, script_pathname
):
    """
    Scale over nodes in a cluster of nodes
    """
    assert benchmark.worker.nr_cluster_nodes_range >= 1
    assert benchmark.worker.nr_numa_nodes_range == 0
    assert benchmark.worker.nr_threads_range == 0

    # Measure the duration of executing an executable on increasingly more
    # nodes. The work size is scaled with the number of nodes in the set.

    # For each number of nodes to use, we create a SLURM script containing
    # the job steps for that partition. Then we create a shell script that
    # executes the SLURM scripts, passing jobs to the scheduler.

    # Iterate over all sets of workers we need to benchmark and format
    # a snippet of bash script for executing the benchmark
    commands = []

    array_shape_per_worker = experiment.array.shape
    partition_shape = experiment.partition.shape

    nr_threads = benchmark.worker.nr_threads

    for benchmark_idx in range(benchmark.worker.nr_benchmarks):
        nr_workers = benchmark.worker.nr_workers(benchmark_idx)
        nr_localities = nr_workers * benchmark.worker.nr_numa_nodes

        array_shape = scale_array_shape(array_shape_per_worker, nr_workers)

        result_pathname = experiment.benchmark_result_pathname(
            result_prefix, cluster.name, benchmark.scenario_name, nr_workers, "json"
        )

        job_steps = [
            # Run the benchmark, resulting in a json file
            f"mpirun --n {nr_tasks} {mpirun_configuration} {experiment.command_pathname} {experiment.command_arguments} "
            # '--hpx:ini="hpx.parcel.mpi.enable=1" '
            # '--hpx:ini="hpx.os_threads={nr_threads}" '
            '--hpx:threads="{nr_threads}" '
            # '--hpx:bind="{thread_binding}" '
            "{program_configuration}".format(
                nr_tasks=nr_localities,
                mpirun_configuration=job.mpirun_configuration(cluster),
                # srun_configuration=job.srun_configuration(cluster),
                nr_threads=nr_threads,
                # thread_binding=util.thread_binding(nr_threads),
                program_configuration=job.program_configuration(
                    result_prefix,
                    cluster,
                    benchmark,
                    experiment,
                    array_shape,
                    partition_shape,
                    nr_workers=nr_workers,
                ),
            )
        ]

        slurm_script = job.create_slurm_script(
            cluster,
            nr_cluster_nodes=nr_workers,
            nr_tasks=nr_localities,
            nr_cores_per_socket=cluster.cluster_node.package.numa_node.nr_cores,
            cpus_per_task=benchmark.nr_logical_cores_per_locality,
            output_filename=experiment.benchmark_result_pathname(
                result_prefix, cluster.name, benchmark.scenario_name, nr_workers, "out"
            ),
            partition_name=cluster.scheduler.settings.partition_name,
            sbatch_options=cluster.scheduler.settings.sbatch_options,
            max_duration=experiment.max_duration,
            job_steps=job_steps,
        )

        job_name = "{name}-{program_name}-{nr_workers}".format(
            name=experiment.name,
            program_name=experiment.program_name,
            nr_workers=nr_workers,
        )
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
            "sbatch --job-name {job_name} {sbatch_options} << {delimiter}".format(
                job_name=job_name,
                sbatch_options=" ".join(cluster.scheduler.settings.sbatch_options),
                delimiter=delimiter,
            ),
            slurm_script,
            "{delimiter}".format(delimiter=delimiter),
            "",
            "# Prevent benchmarks to start at the same time point",
            "sleep 3s",
            "",
        ]

    job.write_script(commands, script_pathname)
    print("bash {}".format(script_pathname))


def generate_script_slurm(
    result_prefix, cluster, benchmark, experiment, script_pathname
):
    assert benchmark.worker.type in ["cluster_node", "numa_node", "thread"]

    if benchmark.worker.nr_cluster_nodes_range > 0:
        generate_script_slurm_cluster_nodes(
            result_prefix, cluster, benchmark, experiment, script_pathname
        )
    elif benchmark.worker.nr_numa_nodes_range > 0:
        generate_script_slurm_numa_nodes(
            result_prefix, cluster, benchmark, experiment, script_pathname
        )
    elif benchmark.worker.nr_threads_range > 0:
        generate_script_slurm_threads(
            result_prefix, cluster, benchmark, experiment, script_pathname
        )


def generate_script_shell(
    result_prefix, cluster, benchmark, experiment, script_pathname
):
    assert benchmark.worker.type == "thread"
    assert benchmark.worker.nr_cluster_nodes_range == 0
    assert benchmark.worker.nr_numa_nodes_range == 0
    assert benchmark.worker.nr_threads_range >= 1

    # Iterate over all sets of workers we need to benchmark and format
    # a snippet of bash script for executing the benchmark
    commands = []

    array_shape_per_worker = experiment.array.shape
    partition_shape = experiment.partition.shape

    for benchmark_idx in range(benchmark.worker.nr_benchmarks):
        nr_workers = benchmark.worker.nr_workers(benchmark_idx)
        nr_threads = nr_workers
        array_shape = scale_array_shape(array_shape_per_worker, nr_workers)
        result_pathname = experiment.benchmark_result_pathname(
            result_prefix, cluster.name, benchmark.scenario_name, nr_workers, "json"
        )

        commands += [
            # Create directory for the resulting json file
            "mkdir -p {}".format(os.path.dirname(result_pathname)),
            # Run the benchmark, resulting in a json file
            f"{experiment.command_pathname} {experiment.command_arguments} "
            # '--hpx:ini="hpx.os_threads={nr_threads}" '
            '--hpx:threads="{nr_threads}" '
            # '--hpx:bind="{thread_binding}" '
            "{program_configuration}".format(
                nr_threads=nr_threads,
                # thread_binding=util.thread_binding(nr_threads),
                program_configuration=job.program_configuration(
                    result_prefix,
                    cluster,
                    benchmark,
                    experiment,
                    array_shape,
                    partition_shape if nr_workers > 1 else array_shape,
                    nr_workers=nr_workers,
                ),
            ),
        ]

    job.write_script(commands, script_pathname)
    print("bash {}".format(script_pathname))


def generate_script(configuration_data):
    """
    Given a fixed array size and partition shape size, iterate over a
    range of sets of workers and capture benchmark results

    A shell script is created that submits jobs to the scheduler. Each
    job executes a benchmark and writes results to a JSON file.
    """

    configuration = Configuration(configuration_data)
    cluster = configuration.cluster
    benchmark = configuration.benchmark
    script_pathname = configuration.script_pathname
    result_prefix = configuration.result_prefix

    assert (
        sum(
            [
                benchmark.worker.nr_cluster_nodes_range > 0,
                benchmark.worker.nr_numa_nodes_range > 0,
                benchmark.worker.nr_threads_range > 0,
            ]
        )
        == 1
    )
    assert benchmark.worker.nr_benchmarks > 0

    experiment = configuration.experiment

    lue_dataset = job.create_raw_lue_dataset(
        result_prefix, cluster, benchmark, experiment
    )
    dataset.write_benchmark_settings(lue_dataset, cluster, benchmark, experiment)

    if cluster.scheduler.kind == "slurm":
        generate_script_slurm(
            result_prefix, cluster, benchmark, experiment, script_pathname
        )
    elif cluster.scheduler.kind == "shell":
        generate_script_shell(
            result_prefix, cluster, benchmark, experiment, script_pathname
        )

    dataset.write_script(lue_dataset, open(script_pathname).read())
