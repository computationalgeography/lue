import os.path
import shutil

import lue.data_model as ldm


### def output_pathname(
###         job_script_pathname):
###
###     return "{}.out".format(job_script_pathname)

###     # template = "{}-{{}}.out".format(job_script_pathname)
###
###     # i = 0
###     # result = template.format(i)
###
###     # while os.path.exists(result):
###     #     i += 1
###     #     result = template.format(i)
###
###     # return result


def mpirun_configuration(cluster):
    """
    Return common arguments to the mpirun command used to start the program
    being benchmarked
    """
    assert cluster.scheduler.kind == "slurm"
    return " ".join(cluster.scheduler.settings.mpirun_options)


def srun_configuration(cluster):
    """
    Return common arguments to the srun command used to start the program
    being benchmarked
    """
    assert cluster.scheduler.kind == "slurm"
    return " ".join(cluster.scheduler.settings.srun_options)


def program_configuration(
    result_prefix,
    cluster,
    benchmark,
    experiment,
    array_shape,
    partition_shape,
    result_pathname=None,
    nr_workers=None,
):
    assert nr_workers is not None

    if result_pathname is None:
        assert not nr_workers is None
        result_pathname = experiment.benchmark_result_pathname(
            result_prefix, cluster.name, benchmark.scenario_name, nr_workers, "json"
        )

    configuration = (
        "--hpx:print-bind "
        '--lue:count="{count}" '
        '--lue:nr_workers="{nr_workers}" '
        '--lue:array_shape="{array_shape}" '
        '--lue:partition_shape="{partition_shape}" '
        '--lue:result="{result_pathname}" '.format(
            count=benchmark.count,
            nr_workers=nr_workers,
            array_shape=list(array_shape),
            partition_shape=list(partition_shape),
            result_pathname=result_pathname,
        )
    )

    #   '--hpx:attach-debugger=exception '
    #   '--hpx:queuing=shared-priority '
    #   '--hpx:queuing=local-priority-fifo '
    #   '--hpx:queuing=static-priority '
    #   '--hpx:queuing=local '
    #   '--hpx:queuing=static '
    #   '--hpx:queuing=abp-priority-fifo '
    #   '--hpx:bind=balanced '
    #   '--hpx:numa-sensitive '
    #   '--hpx:use-process-mask '
    #   '--hpx:bind="{thread_binding}" '
    #   '--hpx:debug-hpx-log '

    # configuration = \
    #     '--hpx:print-bind ' \
    #     '--hpx:ini="hpx.commandline.allow_unknown!=1 ' \
    #     '--hpx:ini="hpx.commandline.aliasing!=0 ' \
    #     '--hpx:ini="hpx.agas.max_pending_refcnt_requests!=50" ' \
    #     '--hpx:ini="hpx.stacks.use_guard_pages=0" ' \
    #     '--hpx:ini="hpx.diagnostics_on_terminate!=0" ' \
    #     '--hpx:ini="application.{program_name}.benchmark.cluster_name!={cluster_name}" ' \
    #     '--hpx:ini="application.{program_name}.benchmark.count!={count}" ' \
    #     '--hpx:ini="application.{program_name}.benchmark.nr_workers!={nr_workers}" ' \
    #     '--hpx:ini="application.{program_name}.benchmark.output!={result_pathname}" ' \
    #     '--hpx:ini="application.{program_name}.nr_time_steps!={nr_time_steps}" ' \
    #     '--hpx:ini="application.{program_name}.array_shape!={array_shape}" ' \
    #     '--hpx:ini="application.{program_name}.partition_shape!={partition_shape}" ' \
    #         .format(
    #             # thread_binding=benchmark.thread_binding,
    #             program_name=experiment.program_name,
    #             cluster_name=cluster.name,
    #             count=benchmark.count,
    #             nr_workers=nr_workers,
    #             nr_time_steps=experiment.nr_time_steps,
    #             array_shape=list(array_shape),
    #             partition_shape=list(partition_shape),
    #             result_pathname=result_pathname,
    #         )

    # arguments = []

    # if experiment.max_tree_depth is not None:
    #     arguments.append(
    #         '--hpx:ini="application.{program_name}.benchmark.max_tree_depth!={max_tree_depth}"'
    #             .format(
    #                 program_name=experiment.program_name,
    #                 max_tree_depth=experiment.max_tree_depth))

    # if not benchmark.hpx is None:

    #     performance_counters = benchmark.hpx.performance_counters

    #     if performance_counters is not None:

    #         assert not nr_workers is None
    #         counter_pathname = experiment.benchmark_result_pathname(
    #             result_prefix, cluster.name, benchmark.scenario_name,
    #             "counter-{}".format(nr_workers), "csv")

    #         # Format arguments for tracking performance counters
    #         arguments += [
    #             '--hpx:print-counter-format=csv',
    #             '--hpx:print-counter-destination="{destination}"'.format(
    #                 destination=counter_pathname),
    #         ]

    #         for argument in performance_counters:
    #             # Each argument is a dict with a single item. Each dict
    #             # maps a key (the argument name) to a value. Value can
    #             # be a single value or a list of values.
    #             assert len(argument) == 1, argument
    #             items = list(argument.items())
    #             assert len(items) == 1
    #             assert len(items[0]) == 2
    #             # assert len(argument.items()[0]) == 2, argument.items()
    #             key, value = items[0]

    #             if not isinstance(value, list):
    #                 arguments.append('--hpx:{}="{}"'.format(key, value))
    #             else:
    #                 for item in value:
    #                     arguments.append('--hpx:{}="{}"'.format(key, item))

    # arguments += experiment.argument_list

    # configuration += " {}".format(" ".join(arguments))

    return configuration


def write_script(commands, script_pathname):
    with open(script_pathname, "w") as script:
        script.write(
            """\
#!/usr/bin/env bash
set -e

{commands}
""".format(
                commands="\n".join(commands)
            )
        )


def memory_requirements(
    cluster, benchmark, experiment, nr_tasks: int
) -> tuple[int, str]:
    """
    Return number of GiB of memory required per cluster node. This amount can be less than what is available
    in a cluster node, or it can be the total amount available per cluster node.
    """
    amount = 0
    binding = ""

    if benchmark.worker.type == "thread":
        assert nr_tasks == 1, nr_tasks

        if benchmark.locality_per == "numa_node":
            amount = cluster.cluster_node.package.numa_node.memory
            binding = "local"
        elif benchmark.locality_per == "cluster_node":
            amount = cluster.cluster_node.memory
            binding = "none"
    elif benchmark.worker.type == "numa_node":
        assert benchmark.locality_per == "numa_node"
        assert nr_tasks <= cluster.cluster_node.nr_numa_nodes, nr_tasks

        if experiment.name == "partition_shape":
            # All memory must be available
            amount = cluster.cluster_node.memory

            # A large problem is solved with all NUMA nodes. Each of them should only use its own memory.
            binding = "local"
        elif experiment.name == "strong_scalability":
            # All memory must be available
            amount = cluster.cluster_node.memory

            # A large problem is solved with increasingly more NUMA nodes. All of them should be able to use
            # all memory
            binding = "none"
        elif experiment.name == "weak_scalability":
            # Limit memory usage to what is available in a NUMA node
            amount = nr_tasks * cluster.cluster_node.package.numa_node.memory

            # An increasingly large problem is solved with increasingly more NUMA nodes. Each of them should
            # only use its own local memory.
            binding = "local"
    elif benchmark.worker.type == "cluster_node":
        assert benchmark.locality_per == "numa_node"
        assert nr_tasks >= cluster.cluster_node.nr_numa_nodes, nr_tasks

        amount = cluster.cluster_node.memory
        binding = "local"  # Since a process per NUMA node is used

    return amount, binding


def create_slurm_script2(
    cluster,
    nr_cluster_nodes,
    nr_tasks,
    benchmark,
    experiment,
    job_steps,
    result_prefix,
):
    """
    Return a snippet of SLURM script configured to allocate the right amount of hardware
    """
    partition_name = cluster.scheduler.settings.partition_name

    # A CPU in SLURM is either a core or a hyper-thread within a core
    # On eejit, a CPU is a thread (hyper threading is on)
    # On snellius, a CPU is a core (hyper threading is off)
    nr_cpus_per_task = benchmark.nr_logical_cores_per_locality

    memory_required_per_cluster_node, memory_binding = memory_requirements(
        cluster, benchmark, experiment, nr_tasks
    )

    # This makes sure that hyper threads are not used
    max_nr_tasks_per_core = 1  # implies --cpu-bind=cores

    sbatch_options = cluster.scheduler.settings.sbatch_options
    max_duration = experiment.max_duration
    software_environment = cluster.software_environment.configuration

    exclusive = (
        True
        if benchmark.worker.type != "thread" and experiment.name == "strong_scalability"
        else False
    )

    return """\
#!/usr/bin/env bash

#SBATCH --partition={partition_name}
#SBATCH --nodes={nr_cluster_nodes}
{exclusive}
#SBATCH --cpus-per-task={nr_cpus_per_task}
#SBATCH --ntasks-per-core={max_nr_tasks_per_core}
#SBATCH --ntasks={nr_tasks}
#SBATCH --mem={memory}G
#SBATCH --mem-bind={memory_binding}
#SBATCH --output={output_filename}
{sbatch_options}
{max_duration}

{software_environment}

{job_steps}""".format(
        partition_name=partition_name,
        nr_cluster_nodes=nr_cluster_nodes,
        exclusive=("#SBATCH --exclusive" if exclusive else ""),
        nr_cpus_per_task=nr_cpus_per_task,
        max_nr_tasks_per_core=max_nr_tasks_per_core,
        nr_tasks=nr_tasks,
        memory=memory_required_per_cluster_node,
        memory_binding=memory_binding,
        sbatch_options="\n".join(
            ["#SBATCH {}".format(option) for option in sbatch_options]
        ),
        max_duration=(
            "#SBATCH --time={}".format(max_duration) if max_duration is not None else ""
        ),
        software_environment=software_environment,
        job_steps="\n".join(job_steps),
        output_filename=experiment.result_pathname(
            result_prefix,
            cluster.name,
            benchmark.scenario_name,
            "slurm",
            f"out-{nr_tasks}",
        ),
    )


def create_slurm_script(
    cluster,
    nr_cluster_nodes,  # How many nodes to reserve
    nr_tasks,  # How many tasks to reserve for
    nr_cores_per_socket,  # Number of physical cores per socket
    # nr_cores_per_numa_node,
    # nr_threads,  # Total nr of threads, including HT threads
    cpus_per_task,
    output_filename,
    partition_name,
    sbatch_options,
    max_duration,
    job_steps,
):
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

    # In SLURM, a CPU is either a core or a thread, depending on the
    # system configuration
    # SBATCH --sockets-per-node=2

    # SBATCH --threads-per-core=1
    ### #SBATCH --cores-per-socket={cores_per_socket}

    return """\
#!/usr/bin/env bash
#SBATCH --nodes={nr_cluster_nodes}
#SBATCH --ntasks={nr_tasks}
#SBATCH --cpus-per-task={cpus_per_task}
#SBATCH --output={output_filename}
#SBATCH --partition={partition_name}
{sbatch_options}
{max_duration}


{software_environment}

{job_steps}""".format(
        nr_cluster_nodes=nr_cluster_nodes,  # Ask for this nr_cluster_nodes
        nr_tasks=nr_tasks,  # Ask for hardware for max nr_tasks
        # cores_per_socket=nr_cores_per_socket,  # Ask for whole sockets of core
        # cores_per_socket=nr_cores_per_numa_node,
        cpus_per_task=cpus_per_task,
        output_filename=output_filename,
        partition_name=partition_name,
        sbatch_options="\n".join(
            ["#SBATCH {}".format(option) for option in sbatch_options]
        ),
        max_duration=(
            "#SBATCH --time={}".format(max_duration) if max_duration is not None else ""
        ),
        software_environment=cluster.software_environment.configuration,
        job_steps="\n".join(job_steps),
    )
    # job_steps="\nsleep 2s\n".join(job_steps))


def lue_raw_dataset_basename():
    return "raw"


def lue_scalability_dataset_basename():
    return "scalability"


def lue_raw_dataset_pathname(result_prefix, cluster, benchmark, experiment):
    return experiment.result_pathname(
        result_prefix,
        cluster.name,
        benchmark.scenario_name,
        lue_raw_dataset_basename(),
        "lue",
    )


def lue_scalability_dataset_pathname(result_prefix, cluster, benchmark, experiment):
    return experiment.result_pathname(
        result_prefix,
        cluster.name,
        benchmark.scenario_name,
        lue_scalability_dataset_basename(),
        "lue",
    )


def create_raw_lue_dataset(result_prefix, cluster, benchmark, experiment):
    dataset_pathname = lue_raw_dataset_pathname(
        result_prefix, cluster, benchmark, experiment
    )
    directory_pathname = os.path.split(dataset_pathname)[0]

    if os.path.exists(directory_pathname):
        assert os.path.isdir(directory_pathname), directory_pathname
    else:
        os.makedirs(directory_pathname)

    assert not os.path.exists(dataset_pathname), dataset_pathname

    dataset = ldm.create_dataset(dataset_pathname, experiment.description)

    return dataset


def open_raw_lue_dataset(result_prefix, cluster, benchmark, experiment, open_mode):
    dataset_pathname = lue_raw_dataset_pathname(
        result_prefix, cluster, benchmark, experiment
    )
    assert os.path.exists(dataset_pathname), dataset_pathname
    dataset = ldm.open_dataset(dataset_pathname, open_mode)

    return dataset


def open_scalability_lue_dataset(
    result_prefix, cluster, benchmark, experiment, open_mode
):
    dataset_pathname = lue_scalability_dataset_pathname(
        result_prefix, cluster, benchmark, experiment
    )
    assert os.path.exists(dataset_pathname), dataset_pathname
    dataset = ldm.open_dataset(dataset_pathname, open_mode)

    return dataset


def scalability_lue_dataset_exists(result_prefix, cluster, benchmark, experiment):
    dataset_pathname = lue_scalability_dataset_pathname(
        result_prefix, cluster, benchmark, experiment
    )
    return os.path.exists(dataset_pathname)


def copy_raw_to_scalability_lue_dataset(result_prefix, cluster, benchmark, experiment):
    raw_dataset_pathname = lue_raw_dataset_pathname(
        result_prefix, cluster, benchmark, experiment
    )
    scalability_dataset_pathname = lue_scalability_dataset_pathname(
        result_prefix, cluster, benchmark, experiment
    )

    shutil.copyfile(raw_dataset_pathname, scalability_dataset_pathname)
