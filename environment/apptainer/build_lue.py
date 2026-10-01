#!/usr/bin/env python3
"""
Script for building LUE and its dependencies (HPX and mdspan)
"""

import contextlib
import logging
import os
import shlex
import shutil
import subprocess
import sys
import tarfile
import tempfile
from pathlib import Path

import docopt
import requests
import tomllib

logger = logging.getLogger(__name__)

CMAKE_ARGUMENTS = [
    "CMAKE_CXX_STANDARD=23",
    "CMAKE_EXPORT_COMPILE_COMMANDS=TRUE",
]


def normalize(path: str, **variables) -> Path:
    """
    Return the path passed in with all custom and environment variables replaced and the result canonicalized

    The variables passed is a dictionary with variable name, variable value mappings. Each occurrence of
    `$variable_name` in path is replaced by the variable value. Regular environment variables are also
    replaced by the their respective values.
    """
    pathname = str(path)

    for name, value in variables.items():
        pathname = pathname.replace(f"${name}", value)

    return Path(os.path.expandvars(pathname)).expanduser().resolve(strict=False)


def clone_branch(url: str, branch: str, path: Path):
    command = f"git clone --depth 1 --branch {branch} {url} {path}"

    subprocess.run(shlex.split(command), check=True)


def clone_tag(url: str, tag: str, path: Path):
    command = f"git clone --depth 1 --branch {tag} {url} {path}"

    subprocess.run(shlex.split(command), check=True)


def clone_commit(url: str, commit: str, path: Path):

    # NOTE: a specific commit, in one go, requires Git >= 2.49 ...
    # command = f"git clone --depth 1 --revision {commit} {url} {path}"

    command = f"git clone {url} {path}"

    subprocess.run(shlex.split(command), check=True)

    with contextlib.chdir(path):
        command = f"git checkout {commit}"

        subprocess.run(shlex.split(command), check=True)


def usage() -> str:
    command = Path(sys.argv[0]).name
    return f"""\
Manage a LUE build

Usage:
    {command} install_hpx <configuration> <build_type> <nr_jobs>
        <source_prefix> <build_prefix> <install_prefix>
    {command} install_mdspan <configuration> <build_type> <nr_jobs>
        <source_prefix> <build_prefix> <install_prefix>
    {command} install_lue <configuration> <build_type> <nr_jobs>
        <source_prefix> <build_prefix> <install_prefix>

Options:
    -h --help          Show this screen and exit

Some conventions:
- If a build directory exists, it is assumed to at least contain a configured
  build. Remove it first if you want to reconfigure.
- If a source directory exists, it is assumed to contain the required sources.
  Remove it first if you want to obtain new sources. If you want to vary
  versions then it make sense to include the version in the name of the source
  directory.
"""


def install_hpx_sources(source_prefix_path: Path, version: str) -> None:
    source_prefix_parent_path = source_prefix_path.parent
    source_prefix_parent_path.mkdir(parents=True, exist_ok=True)

    # A place to store and cache the source zip
    zip_prefix_path = source_prefix_parent_path

    zip_path = zip_prefix_path / f"v{version}.tar.gz"

    # Reuse an existing source zip
    if zip_path.exists():
        logger.info(f"Zip {zip_path} already exists and will be reused")
    else:
        # Download sources zip
        url = (
            f"https://github.com/STEllAR-GROUP/hpx/archive/refs/tags/v{version}.tar.gz"
        )
        response = requests.get(url)

        if response.status_code == 200:
            with open(zip_path, "wb") as file:
                file.write(response.content)
        else:
            raise RuntimeError(f"Failed to download file {url}")

    # Unzip sources zip, in a temp directory. Then move it to the final location.
    with (
        tempfile.TemporaryDirectory() as temp_directory_pathname,
        contextlib.chdir(temp_directory_pathname),
        tarfile.open(zip_path) as tar,
    ):
        tar_info = tar.next()
        assert tar_info is not None
        assert tar_info.type == tarfile.DIRTYPE
        tar.extractall()
        shutil.move(tar_info.name, source_prefix_path)


def configure_hpx_build(
    source_prefix_path: Path, build_prefix_path: Path, build_type: str
) -> None:
    build_prefix_parent_path = build_prefix_path.parent
    build_prefix_parent_path.mkdir(parents=True, exist_ok=True)

    # Common configuration
    cmake_arguments = CMAKE_ARGUMENTS + [
        f"CMAKE_BUILD_TYPE={build_type}",
    ]

    # HPX configuration
    cmake_arguments += [
        "HPX_USE_CMAKE_CXX_STANDARD=TRUE",
        "HPX_WITH_EXAMPLES=FALSE",
        "HPX_WITH_PKGCONFIG=FALSE",
        "HPX_WITH_TESTS=FALSE",
        "HPX_WITH_NETWORKING=TRUE",
        "HPX_WITH_PARCELPORT_TCP=TRUE",
        "HPX_WITH_PARCELPORT_MPI=FALSE",
        "HPX_WITH_HIDDEN_VISIBILITY=TRUE",
        "HPX_WITH_MALLOC=jemalloc",
    ]

    # Build-type specific HPX configuration
    if build_type == "Debug":
        cmake_arguments += []
    elif build_type == "RelWithDebInfo":
        cmake_arguments += [
            "HPX_WITH_TRACY=TRUE",
            "HPX_WITH_FETCH_TRACY=TRUE",
            "HPX_WITH_PARCEL_PROFILING=TRUE",
        ]
    elif build_type == "Release":
        cmake_arguments += []

    cmake_arguments = [f"-D {argument}" for argument in cmake_arguments]

    commands = [
        f"cmake -G 'Ninja' -S {source_prefix_path} -B {build_prefix_path} {' '.join(cmake_arguments)}",
    ]

    for command in commands:
        subprocess.run(shlex.split(command), check=True)


def install_hpx(
    configuration: dict,
    source_prefix_path: Path,
    build_prefix_path: Path,
    install_prefix_path: Path,
    build_type: str,
    nr_jobs: int,
) -> None:
    source_prefix_path = normalize(
        source_prefix_path / configuration["hpx"]["prefix"]["source"]
    )
    build_prefix_path = normalize(
        build_prefix_path / configuration["hpx"]["prefix"]["build"],
        build_type=build_type,
    )
    install_prefix_path = normalize(
        install_prefix_path / configuration["hpx"]["prefix"]["install"],
        build_type=build_type,
    )
    version = configuration["hpx"]["version"]

    if build_prefix_path.exists():
        logger.info(
            f"Build directory {build_prefix_path} already exists and will be reused"
        )
    else:
        if source_prefix_path.exists():
            logger.info(
                f"Source directory {source_prefix_path} already exists and will be reused"
            )
        else:
            install_hpx_sources(source_prefix_path, version)

        configure_hpx_build(source_prefix_path, build_prefix_path, build_type)

    commands = [
        f"cmake --build {build_prefix_path} --parallel {nr_jobs} --target all",
        f"cmake --install {build_prefix_path} --prefix {install_prefix_path} --strip",
    ]

    for command in commands:
        subprocess.run(shlex.split(command), check=True)


def install_mdspan_sources(source_prefix_path: Path, commit: str) -> None:
    source_prefix_parent_path = source_prefix_path.parent
    source_prefix_parent_path.mkdir(parents=True, exist_ok=True)

    url = "https://github.com/kokkos/mdspan.git"

    clone_commit(url, commit, source_prefix_path)


def configure_mdspan_build(
    source_prefix_path: Path, build_prefix_path: Path, build_type: str
) -> None:
    build_prefix_parent_path = build_prefix_path.parent
    build_prefix_parent_path.mkdir(parents=True, exist_ok=True)

    # Common configuration
    cmake_arguments = CMAKE_ARGUMENTS + [
        f"CMAKE_BUILD_TYPE={build_type}",
    ]

    cmake_arguments = [f"-D {argument}" for argument in cmake_arguments]

    commands = [
        f"cmake -G 'Ninja' -S {source_prefix_path} -B {build_prefix_path} {' '.join(cmake_arguments)}",
    ]

    for command in commands:
        subprocess.run(shlex.split(command), check=True)


def install_mdspan(
    configuration: dict,
    source_prefix_path: Path,
    build_prefix_path: Path,
    install_prefix_path: Path,
    build_type: str,
    nr_jobs: int,
) -> None:
    source_prefix_path = normalize(
        source_prefix_path / configuration["mdspan"]["prefix"]["source"]
    )
    build_prefix_path = normalize(
        build_prefix_path / configuration["mdspan"]["prefix"]["build"],
        build_type=build_type,
    )
    install_prefix_path = normalize(
        install_prefix_path / configuration["mdspan"]["prefix"]["install"],
        build_type=build_type,
    )
    commit = configuration["mdspan"]["commit"]

    if build_prefix_path.exists():
        logger.info(
            f"Build directory {build_prefix_path} already exists and will be reused"
        )
    else:
        if source_prefix_path.exists():
            logger.info(
                f"Source directory {source_prefix_path} already exists and will be reused"
            )
        else:
            install_mdspan_sources(source_prefix_path, commit)

        configure_mdspan_build(source_prefix_path, build_prefix_path, build_type)

    commands = [
        f"cmake --build {build_prefix_path} --parallel {nr_jobs} --target all",
        f"cmake --install {build_prefix_path} --prefix {install_prefix_path} --strip",
    ]

    for command in commands:
        subprocess.run(shlex.split(command), check=True)


def install_lue_sources(source_prefix_path: Path, branch: str) -> None:
    source_prefix_parent_path = source_prefix_path.parent
    source_prefix_parent_path.mkdir(parents=True, exist_ok=True)

    url = "https://github.com/computationalgeography/lue.git"

    clone_branch(url, branch, source_prefix_path)


def configure_lue_build(
    source_prefix_path: Path,
    build_prefix_path: Path,
    build_type: str,
    mdspan_prefix_path: Path,
    hpx_prefix_path: Path,
) -> None:
    build_prefix_parent_path = build_prefix_path.parent
    build_prefix_parent_path.mkdir(parents=True, exist_ok=True)

    # Common configuration
    cmake_arguments = CMAKE_ARGUMENTS + [
        f"CMAKE_BUILD_TYPE={build_type}",
        "CMAKE_COMPILE_WARNING_AS_ERROR=TRUE",
        "CMAKE_VERIFY_INTERFACE_HEADER_SETS=TRUE",
    ]

    # LUE configuration
    cmake_arguments += [
        "LUE_BUILD_DOCUMENTATION=TRUE",
        "LUE_DATA_MODEL_WITH_PYTHON_API=TRUE",
        "LUE_DATA_MODEL_WITH_UTILITIES=TRUE",
        # "LUE_FRAMEWORK_WITH_IMAGE_LAND=FALSE",
        "LUE_FRAMEWORK_WITH_PYTHON_API=TRUE",
        "LUE_BUILD_QUALITY_ASSURANCE=TRUE",
        "LUE_QUALITY_ASSURANCE_WITH_PYTHON_API=TRUE",
        "LUE_QUALITY_ASSURANCE_WITH_TESTS=TRUE",
        "LUE_QUALITY_ASSURANCE_TEST_NR_LOCALITIES_PER_TEST=2",
        "LUE_QUALITY_ASSURANCE_TEST_NR_THREADS_PER_LOCALITY=2",
        "LUE_BUILD_VIEW=FALSE",
        "LUE_WITH_EXAMPLES=TRUE",
        f"HPX_ROOT={hpx_prefix_path}",
        f"mdspan_ROOT={mdspan_prefix_path}",
        "Python_EXECUTABLE=/bin/python3",
    ]

    # Build-type-specific LUE configuration
    if build_type == "Debug":
        cmake_arguments += [
            "LUE_ASSERT_CONDITIONS=TRUE",
        ]

    cmake_arguments = [f"-D {argument}" for argument in cmake_arguments]

    commands = [
        f"cmake -G 'Ninja' -S {source_prefix_path} -B {build_prefix_path} {' '.join(cmake_arguments)}",
    ]

    for command in commands:
        subprocess.run(shlex.split(command), check=True)


def install_lue(
    configuration: dict,
    source_prefix_path: Path,
    build_prefix_path: Path,
    install_prefix_path: Path,
    build_type: str,
    nr_jobs: int,
) -> None:
    source_prefix_path = normalize(
        source_prefix_path / configuration["lue"]["prefix"]["source"]
    )
    build_prefix_path = normalize(
        build_prefix_path / configuration["lue"]["prefix"]["build"],
        build_type=build_type,
    )
    lue_install_prefix_path = normalize(
        install_prefix_path / configuration["lue"]["prefix"]["install"],
        build_type=build_type,
    )
    branch = configuration["lue"]["branch"]
    hpx_install_prefix_path = normalize(
        install_prefix_path / configuration["hpx"]["prefix"]["install"],
        build_type=build_type,
    )
    mdspan_install_prefix_path = normalize(
        install_prefix_path / configuration["mdspan"]["prefix"]["install"],
        build_type=build_type,
    )

    if build_prefix_path.exists():
        logger.info(
            f"Build directory {build_prefix_path} already exists and will be reused"
        )
    else:
        if source_prefix_path.exists():
            logger.info(
                f"Source directory {source_prefix_path} already exists and will be reused"
            )
        else:
            install_lue_sources(source_prefix_path, branch)

        configure_lue_build(
            source_prefix_path,
            build_prefix_path,
            build_type,
            mdspan_install_prefix_path,
            hpx_install_prefix_path,
        )

    commands = [
        f"cmake --build {build_prefix_path} --parallel {nr_jobs} --target all",
        f"cmake --install {build_prefix_path} --prefix {lue_install_prefix_path} --strip",
    ]

    for command in commands:
        subprocess.run(shlex.split(command), check=True)


def main() -> int:
    """Command line interface for the bws_random.py script."""
    arguments = docopt.docopt(usage(), sys.argv[1:])

    configuration_file_path = Path(arguments["<configuration>"])
    configuration = tomllib.load(configuration_file_path.open("rb"))

    source_prefix_path = Path(arguments["<source_prefix>"])
    build_prefix_path = Path(arguments["<build_prefix>"])
    install_prefix_path = Path(arguments["<install_prefix>"])
    build_type = arguments["<build_type>"]
    nr_jobs = int(arguments["<nr_jobs>"])

    command_to_function = {
        "install_hpx": install_hpx,
        "install_mdspan": install_mdspan,
        "install_lue": install_lue,
    }
    command = next(command for command in command_to_function if arguments[command])

    logging.basicConfig(level=logging.INFO)

    command_to_function[command](
        configuration,
        source_prefix_path,
        build_prefix_path,
        install_prefix_path,
        build_type,
        nr_jobs,
    )

    return 0


sys.exit(main())
