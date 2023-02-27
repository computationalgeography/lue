#!/usr/bin/env python
import ast
import os.path
import sys

import docopt
import jinja2


def main():

    usage = """\
Read a file, replace some placeholders with values, and write the result
to a file

Usage:
    {command} <source> <destination> <value>

Options:
    source       Pathname of source template
    destination  Pathname of file to generate
    value        Key-value pairs to use for substitutions: Python dictionary
                 formatted as a single string
    -h --help    Show this screen
""".format(
        command=os.path.basename(sys.argv[0])
    )

    arguments = docopt.docopt(usage)

    source_pathname = arguments["<source>"]
    assert os.path.isfile(source_pathname), source_pathname

    destination_pathname = arguments["<destination>"]
    # When this script is called from CMake, we enclose it by single quotes.
    # The VERBATIM option (recommended) of add_custom_command does not strip
    # the quotes from the string. We do it here. This can probably be improved.
    values = dict(ast.literal_eval(arguments["<value>"].strip("'")))

    with open(source_pathname) as source_file:
        template = jinja2.Template(source_file.read())
        open(destination_pathname, "w").write(template.render(values))


if __name__ == "__main__":
    sys.exit(main())
