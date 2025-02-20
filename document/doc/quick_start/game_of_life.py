#!/usr/bin/env python
import sys
import typing
from pathlib import Path

import docopt
import numpy as np

import lue.framework as lfr


Generation = typing.Any
Shape = tuple[int, int]


def initialize_generation(
    array_shape: Shape, partition_shape: Shape | None
) -> Generation:
    fraction_alive_cells = 0.25
    random_field = lfr.uniform(
        array_shape,
        np.dtype(np.float32),
        min_value=0,
        max_value=1,
        partition_shape=partition_shape,
    )
    generation = random_field <= fraction_alive_cells

    return generation


def next_generation(generation: Generation) -> Generation:
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


class GameOfLife(lfr.Model):

    def __init__(
        self, array_shape: Shape, partition_shape: Shape | None, generation_path: Path
    ):
        super().__init__()
        self.array_shape = array_shape
        self.partition_shape = partition_shape
        self.generation_path = generation_path

    def save_generation(self, generation: Generation, generation_id: int) -> None:

        lfr.to_gdal(generation, f"{self.generation_path}-{generation_id}.tif")

    def initialize(self):
        self.generation = initialize_generation(
            array_shape=self.array_shape, partition_shape=self.partition_shape
        )

    def simulate(self, iteration: int):
        self.generation = next_generation(self.generation)
        self.save_generation(self.generation, iteration)

        return self.generation.future()


@lfr.runtime_scope
def game_of_life(
    *,
    array_shape: Shape,
    partition_shape: Shape | None,
    nr_generations: int,
    generation_path: Path,
) -> None:

    model = GameOfLife(
        array_shape=array_shape,
        partition_shape=partition_shape,
        generation_path=generation_path,
    )

    lfr.run_deterministic(
        model, lfr.DefaultProgressor(), nr_time_steps=nr_generations, rate_limit=5
    )


def parse_command_line() -> dict:
    usage = """\
Calculate the generations of alive cells according to the Game of Life
cellular automaton

Usage:
    {command}
        [--partition_shape=<shape>]
        <array_shape> <nr_generations> <pathname>


Options:
    <array_shape>              Shape of the array
    <nr_generations>           Number of Game of Life generations to calculate
    <pathname>                 Pathname of GeoTiffs, without extension
    --partition_shape=<shape>  Shape of the array partitions. If not provided,
                               a default shape will be used.
""".format(
        command=Path(sys.argv[0]).name
    )

    def parse_shape(string: str) -> Shape:
        string = string.strip("[]")
        shape = tuple(int(extent) for extent in string.split(","))
        assert len(shape) == 2, shape
        return shape[0], shape[1]

    # Filter out arguments meant for the HPX runtime
    argv = [arg for arg in sys.argv[1:] if not arg.startswith("--hpx")]
    arguments = docopt.docopt(usage, argv)

    array_shape = parse_shape(arguments["<array_shape>"])
    nr_generations = int(arguments["<nr_generations>"])

    assert nr_generations >= 0, nr_generations

    partition_shape = arguments["--partition_shape"]

    if partition_shape is not None:
        partition_shape = parse_shape(partition_shape)

    path = Path(arguments["<pathname>"])

    return {
        "array_shape": array_shape,
        "partition_shape": partition_shape,
        "nr_generations": nr_generations,
        "generation_path": path,
    }


def main():
    arguments = parse_command_line()
    game_of_life(**arguments)


if __name__ == "__main__":
    main()
