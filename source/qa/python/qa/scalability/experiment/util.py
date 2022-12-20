from lue.qa.scalability.core import json
import dateutil.parser
import os.path


def sort_benchmarks_by_time(
        result_prefix,
        cluster,
        benchmark,
        experiment):

    items = []

    for benchmark_idx in range(benchmark.worker.nr_benchmarks):

        nr_workers = benchmark.worker.nr_workers(benchmark_idx)
        benchmark_pathname = experiment.benchmark_result_pathname(
            result_prefix, cluster.name, benchmark.scenario_name, nr_workers, "json")
        assert os.path.exists(benchmark_pathname), benchmark_pathname
        benchmark_json = json.json_to_data(benchmark_pathname)
        benchmark_start = dateutil.parser.isoparse(benchmark_json["start"])

        items.append((benchmark_start, benchmark_idx))

    assert len(items) > 0

    items.sort(key=lambda item: item[0])

    time_points = [item[0] for item in items]
    idxs = [item[1] for item in items]

    assert all(t1 <= t2 for t1, t2 in zip(time_points, time_points[1:])), time_points
    epoch = time_points[0]

    return idxs, epoch
