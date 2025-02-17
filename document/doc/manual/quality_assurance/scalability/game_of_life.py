#!/usr/bin/env python
import sys
from pathlib import Path

import docopt
import numpy as np

import lue.framework as lfr
import lue.qa.scalability.instrument as lqi


def initialize_generation(array_shape, partition_shape):
    generation = lfr.create_array(
        array_shape=array_shape,
        partition_shape=partition_shape,
        dtype=np.dtype(np.float32),
        fill_value=0,
    )
    fraction_alive_cells = 0.25
    generation = lfr.uniform(generation, np.float32, 0, 1) <= fraction_alive_cells

    return generation


def next_generation(generation):
    kernel = np.array(
        [
            [1, 1, 1],
            [1, 0, 1],
            [1, 1, 1],
        ],
        dtype=np.uint8,
    )
    nr_alive_cells = lfr.focal_sum(generation, kernel)

    # Next state of currently alive cells
    underpopulated = nr_alive_cells < 2
    overpopulated = nr_alive_cells > 3

    # Next state of currently dead cells
    reproducing = nr_alive_cells == 3

    generation = lfr.where(
        generation,
        # True if alive and not under/overpopulated
        ~(underpopulated | overpopulated),
        # True if dead with three neighbours
        reproducing,
    )

    return generation


@lfr.runtime_scope
def game_of_life_scalability(
    count: int,
    nr_workers: int,
    array_shape: tuple[int, int],
    partition_shape: tuple[int, int],
    nr_generations: int,
    result_pathname: str,
) -> None:

    generation = initialize_generation(array_shape, partition_shape)
    generation.future().get()

    experiment = lqi.ArrayExperiment(nr_workers, array_shape, partition_shape)
    experiment.start()

    for _ in range(count):
        run = lqi.Run()

        run.start()

        for _ in range(1, nr_generations + 1):
            generation = next_generation(generation)

        generation.future().get()
        run.stop()

        experiment.add(run)

    experiment.stop()
    lqi.save_results(experiment, result_pathname)


def parse_shape(string):
    string = string.strip("[]")
    return tuple(int(extent) for extent in string.split(","))


def main():
    usage = """\
Calculate the generations of alive cells according to the Game of Life
cellular automaton

Usage:
    {command}
        --lue:count=<count> --lue:nr_workers=<nr_workers>
        --lue:array_shape=<array_shape> --lue:partition_shape=<partition_shape>
        --lue:result=<result>
        <nr_generations>

Options:
    <nr_generations>  Number of Game of Life generations to calculate
""".format(
        command=Path(sys.argv[0]).name
    )

    # Filter out arguments meant for the HPX runtime
    argv = [arg for arg in sys.argv[1:] if not arg.startswith("--hpx")]
    arguments = docopt.docopt(usage, argv)
    nr_generations = int(arguments["<nr_generations>"])
    count = int(arguments["--lue:count"])
    nr_workers = int(arguments["--lue:nr_workers"])
    array_shape = parse_shape(arguments["--lue:array_shape"])
    partition_shape = parse_shape(arguments["--lue:partition_shape"])
    result_pathname = arguments["--lue:result"]

    assert nr_generations >= 0, nr_generations
    assert count > 0, count
    assert nr_workers > 0, nr_workers

    game_of_life_scalability(
        count,
        nr_workers,
        array_shape,
        partition_shape,
        nr_generations,
        result_pathname,
    )


if __name__ == "__main__":
    main()
