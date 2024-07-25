#!/usr/bin/env python
import glob
import os.path
import sys
from pathlib import Path

import docopt
from livereload import Server, shell


# https://pypi.org/project/livereload/


def serve_documentation(
    source_directory_path: Path, build_directory_path: Path
) -> None:
    source_prefix = f"{source_directory_path}/source"
    documentation_source_prefix = f"{source_directory_path}/document/doc"
    documentation_build_prefix = f"{build_directory_path}/document/doc/_build/html"

    server = Server()

    pathnames_to_watch = (
        glob.glob(f"{documentation_source_prefix}/**/*.md", recursive=True)
        + glob.glob(f"{source_prefix}/**/*.md", recursive=True)
        + [
            f"{documentation_source_prefix}/_config.yml.in",
            f"{documentation_source_prefix}/_toc.yml",
            f"{documentation_source_prefix}/CMakeLists.txt",
            f"{documentation_source_prefix}/references.bib",
        ]
    )

    for pathname in pathnames_to_watch:
        server.watch(
            pathname,
            shell(f"cmake --build {build_directory_path} --target documentation"),
        )
    server.serve(root=documentation_build_prefix)


def main() -> None:
    command = os.path.basename(sys.argv[0])
    usage = f"""\
Serve documentation and refresh when source files change

Usage:
    {command} <source_directory> <build_directory>

Arguments:
    source_directory  Pathname of project's source directory
    build_directory   Pathname of project's build directory

Options:
    -h --help         Show this screen and exit
    --version         Show version and exit
"""
    arguments = sys.argv[1:]
    arguments = docopt.docopt(usage, arguments)
    source_directory_path = Path(arguments["<source_directory>"])  # type: ignore
    build_directory_path = Path(arguments["<build_directory>"])  # type: ignore

    serve_documentation(source_directory_path, build_directory_path)


if __name__ == "__main__":
    main()
