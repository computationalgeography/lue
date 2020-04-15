#!/usr/bin/env python
import os.path
import sys
sys.path = [
    os.path.join(os.path.split(__file__)[0], "..", "..", "benchmark", "lue")
] + sys.path
from benchmark import export_results
import docopt


usage = """\
Export benchmark results

Usage:
    {command} <dataset> <csv_file>

Options:
    dataset    Pathname of dataset containing benchmark results
    csv_file   Pathname of CSV file to write output to
    -h --help  Show this screen
""".format(
    command = os.path.basename(sys.argv[0]))


if __name__ == "__main__":
    arguments = docopt.docopt(usage)

    lue_dataset_pathname = arguments["<dataset>"]
    csv_file_pathname = arguments["<csv_file>"]
    assert os.path.isfile(lue_dataset_pathname), lue_dataset_pathname

    export_results(lue_dataset_pathname, csv_file_pathname)
