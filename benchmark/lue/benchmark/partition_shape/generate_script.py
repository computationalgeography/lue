from .benchmark import *
from .experiment import *
from ..cluster import *
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


def generate_script_slurm(
        cluster,
        benchmark,
        experiment,
        script_pathname):

    output_filename = job_output_pathname(script_pathname)
    partition_shapes = experiment.partition_shapes()

    # Iterate over all partition shapes we need to benchmark and format
    # a snippet of bash script for executing it
    commands = []

    for partition_shape in partition_shapes:
        result_pathname = experiment.benchmark_result_pathname(partition_shape)

        commands.append(
            "srun {command_pathname} "
                '--hpx:ini="hpx.parcel.mpi.enable=1" '
                '--hpx:ini="hpx.parcel.tcp.enable=0" '

                # TODO Refactor this part with the twin function
                '--hpx:ini="application.{program_name}.benchmark.count!={count}" '
                '--hpx:ini="application.{program_name}.benchmark.max_tree_depth!={max_tree_depth}" '
                '--hpx:ini="application.{program_name}.benchmark.output!={result_pathname}" '
                '--hpx:ini="application.{program_name}.nr_time_steps!={nr_time_steps}" '
                '--hpx:ini="application.{program_name}.array_shape!={array_shape}" '
                '--hpx:ini="application.{program_name}.partition_shape!={partition_shape}" '
                # TODO /Refactor this part with the twin function
            .format(
                command_pathname=experiment.command_pathname,
                program_name=experiment.program_name,
                count=benchmark.count,
                max_tree_depth=experiment.max_tree_depth,
                nr_time_steps=experiment.nr_time_steps,
                array_shape=experiment.array_shape,
                partition_shape=partition_shape,
                result_pathname=result_pathname,
            )
        )

    with open(script_pathname, "w") as script:
        script.write("""\
#!/usr/bin/env bash
set -e

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


def generate_script_shell(
        cluster,
        benchmark,
        experiment,
        script_pathname):

    partition_shapes = experiment.partition_shapes()

    # Iterate over all partition shapes we need to benchmark and format
    # a snippet of bash script for executing it
    commands = []

    # TODO Pass in the hpx arguments for setting the number of threads

    for partition_shape in partition_shapes:
        result_pathname = experiment.benchmark_result_pathname(partition_shape)

        commands.append(
            "{command_pathname} "
                '--hpx:ini="hpx.os_threads={nr_threads}" '

                # TODO Refactor this part with the twin function
                '--hpx:ini="application.{program_name}.benchmark.cluster_name!={cluster_name}" '
                '--hpx:ini="application.{program_name}.benchmark.count!={count}" '
                '--hpx:ini="application.{program_name}.benchmark.max_tree_depth!={max_tree_depth}" '
                '--hpx:ini="application.{program_name}.benchmark.output!={result_pathname}" '

                '--hpx:ini="application.{program_name}.nr_time_steps!={nr_time_steps}" '
                '--hpx:ini="application.{program_name}.array_shape!={array_shape}" '
                '--hpx:ini="application.{program_name}.partition_shape!={partition_shape}" '
                # TODO /Refactor this part with the twin function
            .format(
                command_pathname=experiment.command_pathname,
                nr_threads=benchmark.nr_threads,
                program_name=experiment.program_name,
                cluster_name=cluster.name,
                count=benchmark.count,
                max_tree_depth=experiment.max_tree_depth,
                nr_time_steps=experiment.nr_time_steps,
                array_shape=list(experiment.array_shape),
                partition_shape=list(partition_shape),
                result_pathname=result_pathname,
            )
        )

    with open(script_pathname, "w") as script:
        script.write("""\
#!/usr/bin/env bash
set -e

{commands}
""".format(
            commands="\n".join(commands)
        ))

    print("bash ./{}".format(script_pathname))


def generate_script(
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
    experiment = Experiment(experiment_settings_json, command_pathname)

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
