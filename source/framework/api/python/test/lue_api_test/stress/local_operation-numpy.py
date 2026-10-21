#!/usr/bin/env python
import sys
from pathlib import Path

import docopt
import numpy as np


Shape = tuple[int, int]


def parse_shape(string: str) -> Shape:
    string = string.strip("[]")
    shape = tuple(int(extent) for extent in string.split(","))
    assert len(shape) == 2, shape
    return shape[0], shape[1]


def parse_command_line() -> dict:
    usage = """\
Run a model

Usage:
    {command} <array_shape> <nr_time_steps>
""".format(command=Path(sys.argv[0]).name)

    arguments = docopt.docopt(usage, sys.argv[1:])
    array_shape = parse_shape(arguments["<array_shape>"])
    nr_time_steps = int(arguments["<nr_time_steps>"])

    assert nr_time_steps >= 0, nr_time_steps

    return {
        "array_shape": array_shape,
        "nr_time_steps": nr_time_steps,
    }


def run_model(
    *,
    array_shape: Shape,
    nr_time_steps: int,
) -> None:

    state = np.random.uniform(size=array_shape).astype(np.float32)

    for time_step in range(nr_time_steps):
        state = state + state - state + state - state


def main():
    arguments = parse_command_line()
    run_model(**arguments)


if __name__ == "__main__":
    main()
