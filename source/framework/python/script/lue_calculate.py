#!/usr/bin/env python
import ast
import os.path
import sys

import docopt

import lue.framework as lfr
from lue import __version__ as lue_version


# TODO partition shape can be None
# TODO nr_threads can be zero

# TODO Use a smart default partition size. Add a function for that to the C++ code and use it
#      everywhere as a default partition size.


def print_line() -> None:
    print(20 * "-")


def print_ast(tree: ast.AST) -> None:
    print(ast.dump(tree))


def has_extension(pathname: str) -> bool:
    return os.path.splitext(pathname)[1] != ""


def remove_extensions(basename: str) -> str:
    basename, extension = os.path.splitext(basename)

    while extension != "":
        basename, extension = os.path.splitext(basename)

    return basename


class NormalizePathnamesVisitor(ast.NodeTransformer):
    def visit_Constant(self, node):
        if isinstance(node.value, str):
            pathname = os.path.expandvars(node.value)

            if not has_extension(pathname):
                pathname = f"{pathname}.tif"

            node.value = pathname

        return node


class UniqueStringsVisitor(ast.NodeTransformer):
    def __init__(self):
        self.strings = set()

    def visit_Constant(self, node):
        if isinstance(node.value, str):
            self.strings.add(node.value)

        return node


class ReplacePathnamesVisitor(ast.NodeTransformer):
    def __init__(self, variable_name_by_pathname):
        self.variable_name_by_pathname = variable_name_by_pathname

    def visit_Constant(self, node):
        if node.value in self.variable_name_by_pathname:
            node = ast.Name(
                id=self.variable_name_by_pathname[node.value], ctx=ast.Load()
            )

        return node


class QualifyOperationVisitor(ast.NodeTransformer):
    def visit_Call(self, node):
        # Visit child-nodes first
        self.generic_visit(node)

        node.func.id = f"lfr.{node.func.id}"
        return node


def pathname_to_variable_name(pathname: str) -> str:
    basename = os.path.basename(pathname)
    variable_name = remove_extensions(basename)

    return variable_name


def pathnames_to_variable_names(pathnames: set[str]) -> dict[str, str]:
    variable_name_by_pathname: dict[str, str] = {}  # type: ignore

    for pathname in pathnames:
        variable_name = pathname_to_variable_name(pathname)
        i = 0

        while f"{variable_name}_{i}" in variable_name_by_pathname.values():
            i += 1

        variable_name = f"{variable_name}_{i}"
        variable_name_by_pathname[pathname] = variable_name

    return variable_name_by_pathname


def statement_to_statement_block(
    statement: str, partition_shape: tuple[int, int]
) -> list[str]:
    """
    Parse statement and create a block of code
    """

    assert "=" in statement, statement
    lhs, rhs = statement.split("=")
    lhs = lhs.strip()
    lhs = ", ".join(f'"{name.strip()}"' for name in lhs.split(","))
    rhs = rhs.strip()

    lhs_tree = ast.parse(lhs)
    rhs_tree = ast.parse(rhs)

    normalize_pathnames_visitor = NormalizePathnamesVisitor()
    rhs_tree = normalize_pathnames_visitor.visit(rhs_tree)
    lhs_tree = normalize_pathnames_visitor.visit(lhs_tree)

    # Collect all unique input strings in the rhs expression
    unique_strings_visitor = UniqueStringsVisitor()
    rhs_tree = unique_strings_visitor.visit(rhs_tree)

    # Assume the strings are pathnames of raster datasets. Create variable names for each of them.
    variable_name_by_pathname = pathnames_to_variable_names(
        unique_strings_visitor.strings
    )

    # Prepend the original statement with from_gdal statements. Name variable after uniquified
    # basename without extension of string.
    read_statements = []

    for pathname, variable_name in variable_name_by_pathname.items():
        read_statements.append(
            f'{variable_name} = lfr.from_gdal("{pathname}", partition_shape={partition_shape})'
        )

    clone_pathname = list(variable_name_by_pathname)[0]

    # Replace all occurrences of each string in original statement with the associated
    # variable name
    replace_pathnames_visitor = ReplacePathnamesVisitor(variable_name_by_pathname)
    rhs_tree = replace_pathnames_visitor.visit(rhs_tree)

    # Qualify all operations by lfr
    qualify_operation_visitor = QualifyOperationVisitor()
    rhs_tree = qualify_operation_visitor.visit(rhs_tree)

    # Collect all unique output strings in the statement (lhs assignment statement)
    unique_strings_visitor = UniqueStringsVisitor()
    lhs_tree = unique_strings_visitor.visit(lhs_tree)

    # Assume the strings are pathnames of raster datasets. Create variable names for each of them.
    variable_name_by_pathname = pathnames_to_variable_names(
        unique_strings_visitor.strings
    )

    # Replace all occurrences of each string in original statement with the associated
    # variable name.
    replace_pathnames_visitor = ReplacePathnamesVisitor(variable_name_by_pathname)
    lhs_tree = replace_pathnames_visitor.visit(lhs_tree)

    # Append original statement with to_gdal statements. Pass in the name of a clone raster.
    write_statements = []

    for pathname, variable_name in variable_name_by_pathname.items():
        write_statements.append(
            f'lfr.to_gdal({variable_name}, "{pathname}", clone_name="{clone_pathname}")'
        )

    return (
        read_statements
        + [f"{ast.unparse(lhs_tree)} = {ast.unparse(rhs_tree)}"]
        + write_statements
    )


@lfr.runtime_scope
def execute_statement_block(statement_block: list[str]) -> None:
    """
    Execute a statement block within an HPX runtime scope

    The statements have (only) access to functionality in the lue.framework
    subpackage, aliased as lfr.
    """
    exec(";".join(statement_block), {"__builtins__": {}, "lfr": lfr}, {})


def execute(statement: str, partition_shape: tuple[int, int]) -> int:
    statement_block = statement_to_statement_block(statement, partition_shape)

    execute_statement_block(statement_block)

    return 0


def main() -> int:
    usage = """\
Translate input rasters to output rasters using LUE framework

Usage:
    {command} [options] <statement>
    {command} -h | --help
    {command} --version

Options:
    statement            Assignment statement to evaluate
    -h --help            Show this screen
    --version            Show version
    --partition=<shape>  Shape of partitions [default: 500,500]

The right-hand side expression of the assignment statement must be a
valid Python expression. It is assumed that all functions and operators
used are available in the lue.framework subpackage. Other functionality
cannot be used.

The left-hand side expression must be a comma-separated sequence of one
or more pathnames of output rasters. The number of names must correspond
with the number of output rasters output by the right-hand side expression.
Raster names don't need to be quoted.

Environmental variables formatted as $NAME or ${{NAME}} are expanded. In
addition, on Windows variables formatted as %NAME% are also expanded.

Using file extensions is optional. When not present .tif is assumed.

Examples:
    {command} 'slope.tif = slope("$MY_DATA/elevation.tif", 100.0)'
    {command} 'flux, state =
        accu_threshold("flow_direction.tif", "material.tif", "threshold.tif")'
""".format(
        command=os.path.basename(sys.argv[0])
    )

    arguments = [arg for arg in sys.argv[1:] if not arg.startswith("--hpx:")]
    arguments = docopt.docopt(usage, arguments, version=lue_version)
    statement = arguments["<statement>"]  # type: ignore
    partition_shape: tuple[int, int] = tuple(
        int(extent) for extent in arguments["--partition"].split(",")  # type: ignore
    )

    return execute(statement, partition_shape)


if __name__ == "__main__":
    sys.exit(main())
