import itertools
import os.path


def job_output_pathname(
        job_script_pathname):

    return "{}.out".format(job_script_pathname)

    # template = "{}-{{}}.out".format(job_script_pathname)

    # i = 0
    # result = template.format(i)

    # while os.path.exists(result):
    #     i += 1
    #     result = template.format(i)

    # return result


def partition_shape_multipliers(
        shape,
        partition_shape):
    """
    Given an array shape and a partition shape, determine for each dimension
    how many times the partition's extent can fit in the shape's extent.

    Returns a list of multipliers: for each dimension one.
    """

    rank = len(shape)
    assert len(partition_shape) == rank

    assert all([extent > 0 for extent in shape])
    assert all([extent > 0 for extent in partition_shape])

    for r in range(rank):
        assert shape[r] % partition_shape[r] == 0

    shape_multipliers = [shape[r] // partition_shape[r] for r in range(rank)]

    assert all(
        [isinstance(multiplier, int) for multiplier in shape_multipliers])

    return shape_multipliers


def ranges_of_partition_shape_multipliers(
        shape,
        min_partition_shape,
        max_partition_shape):
    """
    Return for each dimension a range of multipliers
    """
    min_partition_shape_multipliers = partition_shape_multipliers(
        shape, min_partition_shape)
    max_partition_shape_multipliers = partition_shape_multipliers(
        shape, max_partition_shape)

    rank = len(shape)

    assert all([
        min_partition_shape_multipliers[r] >=
        max_partition_shape_multipliers[r] for r in range(rank)])

    multiplier_ranges = [
            range(
                min_partition_shape_multipliers[r],
                max_partition_shape_multipliers[r], -1) for r in range(rank)]

    assert len(multiplier_ranges) == rank

    return multiplier_ranges


def partition_shape_ranges(
        min_partition_shape,
        max_partition_shape,
        step):

    assert len(min_partition_shape) == len(max_partition_shape)
    assert step > 0
    rank = len(min_partition_shape)
    assert rank > 0
    assert all(
        [min_partition_shape[r] <= max_partition_shape[r] for r in range(rank)])

    return [
        range(
            min_partition_shape[r],
            max_partition_shape[r] + 1,
            step)
        for r in range(rank)]


class Benchmark(object):

    def __init__(self, json):
        # Parse benchmark settings JSON
        self.count = json["count"]
        self.nr_workers = json["nr_workers"]
        self.worker_type = json["worker_type"]
        assert self.worker_type in ["thread", "node"]

        if self.worker_type == "thread":
            self.nr_nodes = 1
            self.nr_threads = self.nr_workers
        elif worker_type == "node":
            self.nr_nodes = self.nr_workers
            assert False
            self.nr_threads = json["nr_threads"]


class Experiment(object):

    def __init__(self, json):
        # Parse experiment settings JSON
        self.max_duration = json["max_duration"]
        self.max_tree_depth = json["max_tree_depth"]
        self.nr_time_steps = json["nr_time_steps"]
        self.array_shape = tuple(json["array_shape"])
        self.min_partition_shape = tuple(json["min_partition_shape"])
        self.max_partition_shape = tuple(json["max_partition_shape"])
        self.partition_shape_step = json["partition_shape_step"]

    def partition_shapes(self):
        return itertools.product(
            *partition_shape_ranges(
                self.min_partition_shape, self.max_partition_shape,
                self.partition_shape_step))


def partition_shape_slurm(
        cluster,
        benchmark,
        experiment,
        script_pathname,
        command_pathname):

    program_name = os.path.basename(command_pathname)
    output_filename = job_output_pathname(script_pathname)
    partition_shapes = experiment.partition_shapes()

    # Iterate over all partition shapes we need to benchmark and format
    # a snippet of bash script for executing it
    commands = []

    for partition_shape in partition_shapes:
        benchmark_result_pathname = \
            "{}-{}.json".format(program_name, "-".join(
                [str(extent) for extent in partition_shape]))

        commands.append(
            "srun {command_pathname} "
                '--hpx:ini="hpx.parcel.mpi.enable=1" '
                '--hpx:ini="hpx.parcel.tcp.enable=0" '

                # TODO Refactor this part with the twin function
                '--hpx:ini="application.{program_name}.benchmark.count!={count}" '
                '--hpx:ini="application.{program_name}.benchmark.output!={benchmark_result_pathname}" '
                '--hpx:ini="application.{program_name}.max_tree_depth!={max_tree_depth}" '
                '--hpx:ini="application.{program_name}.nr_time_steps!={nr_time_steps}" '
                '--hpx:ini="application.{program_name}.array_shape!={array_shape}" '
                '--hpx:ini="application.{program_name}.partition_shape!={partition_shape}" '
                # TODO /Refactor this part with the twin function
            .format(
                command_pathname=command_pathname,
                program_name=program_name,
                count=benchmark.count,
                max_tree_depth=experiment.max_tree_depth,
                nr_time_steps=experiment.nr_time_steps,
                array_shape=experiment.array_shape,
                partition_shape=partition_shape,
                benchmark_result_pathname=benchmark_result_pathname,
            )
        )

    ### '--hpx:ini="application.{program_name}.benchmark.work_size!={work_size}" '

    with open(script_pathname, "w") as script:
        script.write("""\
#!/usr/bin/env bash
#SBATCH -N {nr_nodes}
#SBATCH -n {nr_nodes}
#SBATCH -c {nr_threads}
#SBATCH --output {output_filename}
#SBATCH --partition {partition_name}
#SBATCH --time {max_duration}

module purge

{commands}

""".format(
            nr_nodes=benchmark.nr_nodes,
            nr_threads=benchmark.nr_threads,
            output_filename=output_filename,
            partition_name=cluster.partition_name,
            max_duration=experiment.max_duration,
            commands="\n".join(commands)
        ))

    print("sbatch {}".format(script_pathname))


def partition_shape_shell(
        cluster,
        benchmark,
        experiment,
        script_pathname,
        command_pathname):

    program_name = os.path.basename(command_pathname)
    partition_shapes = experiment.partition_shapes()

    # Iterate over all partition shapes we need to benchmark and format
    # a snippet of bash script for executing it
    commands = []

    # TODO Pass in the hpx arguments for setting the number of threads

    for partition_shape in partition_shapes:
        benchmark_result_pathname = \
            "{}-{}.json".format(program_name, "-".join(
                [str(extent) for extent in partition_shape]))

        commands.append(
            "{command_pathname} "
                # TODO Refactor this part with the twin function
                '--hpx:ini="application.{program_name}.benchmark.count!={count}" '
                '--hpx:ini="application.{program_name}.benchmark.output!={benchmark_result_pathname}" '
                '--hpx:ini="application.{program_name}.max_tree_depth!={max_tree_depth}" '
                '--hpx:ini="application.{program_name}.nr_time_steps!={nr_time_steps}" '
                '--hpx:ini="application.{program_name}.array_shape!={array_shape}" '
                '--hpx:ini="application.{program_name}.partition_shape!={partition_shape}" '
                # TODO /Refactor this part with the twin function
            .format(
                command_pathname=command_pathname,
                program_name=program_name,
                count=benchmark.count,
                max_tree_depth=experiment.max_tree_depth,
                nr_time_steps=experiment.nr_time_steps,
                array_shape=experiment.array_shape,
                partition_shape=partition_shape,
                benchmark_result_pathname=benchmark_result_pathname,
            )
        )

    with open(script_pathname, "w") as script:
        script.write("""\
#!/usr/bin/env bash

{commands}
""".format(
            commands="\n".join(commands)
        ))

    print("bash ./{}".format(script_pathname))


class SlurmCluster(object):

    def __init__(self, json):

        self.partition_name = json["partition_name"]


class ShellCluster(object):

    def __init__(self, json):

        pass


def partition_shape(
        cluster_settings_json,
        benchmark_settings_json,
        experiment_settings_json,
        script_pathname,
        command_pathname):
    """
    Given a set of workers, iterate over shape sizes and capture
    benchmark results

    A shell script is created that submits jobs to the scheduler. Each
    job executes a benchmark and writes results to a JSON file.
    """
    job_scheduler = cluster_settings_json["job_scheduler"]
    assert job_scheduler in ["shell", "slurm"]

    benchmark = Benchmark(benchmark_settings_json)
    experiment = Experiment(experiment_settings_json)

    if job_scheduler == "slurm":
        cluster = SlurmCluster(cluster_settings_json)
        partition_shape_slurm(
            cluster, benchmark, experiment,
            script_pathname, command_pathname)
    elif job_scheduler == "shell":
        cluster = ShellCluster(cluster_settings_json)
        partition_shape_shell(
            cluster, benchmark, experiment,
            script_pathname, command_pathname)
