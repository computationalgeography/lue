#!/usr/bin/env python
import functools
import os.path
import re
import sys
import traceback

import docopt


def checked_call(function):
    @functools.wraps(function)
    def wrapper(*args, **kwargs):
        result = 0
        try:
            result = function(*args, **kwargs)
        except:
            traceback.print_exc(file=sys.stderr)
            result = 1
        return 0 if result is None else result

    return wrapper


doc_string = """\
Edit a dot formatted graph

Usage:
    {command} [--output=<file>] node add_attribute
        <graph_name> <node_name> <attribute_name> <attribute_value>

Options:
    -h --help        Show this screen
    --version        Show version
    --output=<file>  Name of file to store result in
    graph_name       Pathname of file containing graph
    node_name        Name of node to update
    attribute_name   Name of attribute
    attribute_value  Value of attribute
""".format(
    command=os.path.basename(sys.argv[0])
)


@checked_call
def add_attribute(graph_name, output_name, node_name, attribute_name, attribute_value):

    graph = open(graph_name, "r").read()
    snippet = "{attribute}={value}".format(
        attribute=attribute_name, value=attribute_value
    )
    pattern = r"^\s*{node}\s*\[".format(node=node_name)

    def update_node(match_object):
        return "{match}\n{indent}{attribute}".format(
            match=match_object.group(0), indent=8 * " ", attribute=snippet
        )

    graph, nr_subs = re.subn(pattern, update_node, graph, flags=re.MULTILINE)

    if nr_subs == 0:
        raise RuntimeError("node '{}' was not found".format(node_name))
    elif nr_subs > 1:
        raise RuntimeError("node '{}' was found multiple times".format(node_name))

    stream = sys.stdout if output_name is None else open(output_name, "w")

    stream.write(graph)


if __name__ == "__main__":
    arguments = docopt.docopt(doc_string)

    output_name = arguments["--output"]

    if arguments["node"]:
        if arguments["add_attribute"]:
            graph_name = arguments["<graph_name>"]
            node_name = arguments["<node_name>"]
            attribute_name = arguments["<attribute_name>"]
            attribute_value = arguments["<attribute_value>"]
            function = add_attribute
            arguments = (
                graph_name,
                output_name,
                node_name,
                attribute_name,
                attribute_value,
            )

    sys.exit(function(*arguments))
