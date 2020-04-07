import pandas as pd
import dateutil.parser
import json
import math
import os.path
import shlex
import subprocess
import tempfile


def execute_command(
        command):

    result = subprocess.call(shlex.split(command))

    if result != 0:
        raise RuntimeError("Error executing {}".format(command))


def lue_translate():
    return os.path.expandvars("$LUE_OBJECTS/bin/lue_translate")


def import_lue_json(
        lue_json_pathname,
        lue_dataset_pathname):

    command = "{} import --add {} {}".format(
        lue_translate(),
        lue_dataset_pathname,
        lue_json_pathname)
    execute_command(command)


def create_dot_graph(
        lue_dataset_pathname,
        pdf_graph_pathname):
    """
    Create a dot graph of the LUE file containing the experiment results
    """

    dot_properties_pathname = os.path.expandvars(
        "$LUE/document/lue_translate/dot_properties.json")

    with tempfile.NamedTemporaryFile(suffix=".dot") as dot_graph_file:
        commands = []
        commands.append(
            "{} export --meta {} {} {}".format(
                lue_translate(),
                dot_properties_pathname,
                lue_dataset_pathname,
                dot_graph_file.name))
        commands.append(
            "dot -Tpdf -o {} {}".format(
                pdf_graph_pathname,
                dot_graph_file.name))

        for command in commands:
            execute_command(command)


# millnames = ['',' thousand',' Million',' Billion',' Trillion']
# 
# def millify(n):
#     n = float(n)
#     millidx = max(0,min(len(millnames)-1,
#     int(math.floor(0 if n == 0 else math.log10(abs(n))/3))))
# 
#     return '{:.0f}{}'.format(n / 10**(3 * millidx), millnames[millidx])


def format_duration(
        duration):
    # TODO Pass in units and be smarter

    return duration

    # return "{:,}".format(int(duration))


def format_nr_workers(
        size):
    nr_workers = math.floor(size)
    return "{:,}".format(int(nr_workers)) if nr_workers == size else ""


def format_partition_size(
        size):
    partition_size = math.floor(size)
    return "{:,}".format(int(partition_size)) if partition_size == size else ""


def select_data_for_plot(
        data_frame,
        name,
        count):
    # Select data needed for plotting
    result = data_frame.filter(
        items=
            ["nr_workers"] +
            ["{}_{}".format(name, i) for i in range(count)])

    # Durations per nr workers
    result = result.set_index(keys="nr_workers")
    result = pd.DataFrame(
        data=result.stack(),
        columns=[name])

    # Get rid of introduced level of index
    result.index = result.index.droplevel(1)

    # Create a new index, moving nr_workers index level into columns
    result = result.reset_index()

    return result


def json_to_data(
        pathname):

    pathname = os.path.expandvars(pathname)

    assert os.path.isfile(pathname), pathname

    lines = open(pathname).readlines()
    lines = "".join(
        [line for line in lines if not line.strip().startswith("#")])

    return json.loads(lines)


def sort_benchmarks_by_time(
        cluster,
        benchmark,
        experiment):

    items = []

    for benchmark_idx in range(benchmark.worker.nr_benchmarks):

        nr_workers = benchmark.worker.nr_workers(benchmark_idx)
        benchmark_pathname = experiment.benchmark_result_pathname(
            cluster.name, benchmark.scenario_name, nr_workers, "json")
        assert os.path.exists(benchmark_pathname), benchmark_pathname
        benchmark_json = json_to_data(benchmark_pathname)
        benchmark_start = dateutil.parser.isoparse(benchmark_json["start"])

        items.append((benchmark_start, benchmark_idx))

    assert len(items) > 0

    items.sort(key=lambda item: item[0])
    epoch = items[0][0]
    idxs = [item[1] for item in items]

    return idxs, epoch


def thread_binding(
        nr_threads):

    # Bind OS threads to the first processing unit of each core
    return "thread:0-{}=core:0-{}.pu:0".format(
        nr_threads-1,
        nr_threads-1)
