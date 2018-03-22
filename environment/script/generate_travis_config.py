#!/usr/bin/env python
from collections import namedtuple
import os.path
import sys
import docopt
import devbase


doc_string = """\
Generate a configuration file for Travis

Usage:
    {command}

Options:
    -h --help  Show this screen
""".format(
    command = os.path.basename(sys.argv[0]))


Package = namedtuple("Package", (
        "name",
        "apt_sources",
        "additional_packages",
    ))


Compiler = namedtuple("Compiler", (
        "name",
        "package",
        "flags",
    ))

Compilers = namedtuple("Compilers", (
        "name",
        "c_compiler",
        "cxx_compiler",
    ))

BuildConfiguration = namedtuple("BuildConfiguration", (
        "environment",
        "packages",
    ))

class Build(namedtuple("Build", (
            "compilers",
            "python_version",
            "build_type",
            "packages",
            "environment",
        ))):

    def __repr__(self):
        apt_sources = []
        package_names = []

        apt_sources += self.compilers.c_compiler.package.apt_sources
        package_names.append(self.compilers.c_compiler.package.name)

        apt_sources += self.compilers.cxx_compiler.package.apt_sources
        package_names.append(self.compilers.cxx_compiler.package.name)

        for package in self.packages:
            apt_sources += package.apt_sources
            package_names.append(package.name)
            package_names += package.additional_packages

        apt_sources = list(set(apt_sources))
        package_names = list(set(package_names))

        environment = [
            "TRAVIS_C_COMPILER={}".format(self.compilers.c_compiler.name),
            "TRAVIS_CXX_COMPILER={}".format(self.compilers.cxx_compiler.name),
            "TRAVIS_CXX_FLAGS=\"{}\"".format(self.compilers.cxx_compiler.flags),
            "TRAVIS_BUILD_TYPE={}".format(self.build_type),
            "TRAVIS_LUE_CMAKE_ARGUMENTS=\"-DCMAKE_BUILD_TYPE=$TRAVIS_BUILD_TYPE -DCMAKE_C_COMPILER=$TRAVIS_C_COMPILER -DCMAKE_CXX_COMPILER=$TRAVIS_CXX_COMPILER -DCMAKE_CXX_FLAGS=$TRAVIS_CXX_FLAGS -DLUE_BUILD_TEST:BOOL=TRUE {}\"".format(" ".join(["-D{}={}".format(key, self.environment[key]) for key in self.environment]))
        ]

        return """\
- compiler: {compiler}
  python: "{python_version}"
  addons:
      apt:
          sources:
              - {apt_sources}
          packages:
              - {packages}
  env:
      - {environment}
""".format(
            compiler=self.compilers.name,
            python_version=self.python_version,
            apt_sources="\n              - ".join(apt_sources),
            packages="\n              - ".join(package_names),
            environment=" ".join(environment)
        )


def merge_dicts(
        dict1,
        dict2):
    result = dict1.copy()
    result.update(dict2)
    return result


def builds():

    # A build configuration consists of a combination of:
    # - compiler versions
    # - python version
    # - project settings (CMake variables)


    # A compiler version depends on:
    # - name of apt repositories
    # - name of packages to install

    # A compiler version triggers:
    # - variables to be set, identifying the compilers

    # A project depends on:
    # - name of packages to install


    # These collections are lookup tables. -------------------------------------
    packages = [
        Package("clang-3.9", ["llvm-toolchain-trusty-3.9"], []),
        Package("clang++-3.9", [
            "llvm-toolchain-trusty-3.9",
            "ubuntu-toolchain-r-test"
        ], ["libc++-dev"]),

        Package("clang-4.0", ["llvm-toolchain-trusty-4.0"], []),
        Package("clang++-4.0", [
            "llvm-toolchain-trusty-4.0",
            "ubuntu-toolchain-r-test"
        ], ["libc++-dev"]),

        Package("clang-5.0", ["llvm-toolchain-trusty-5.0"], []),
        Package("clang++-5.0", [
            "llvm-toolchain-trusty-5.0",
            "ubuntu-toolchain-r-test"
        ], ["libc++-dev"]),

        Package("clang-6.0", ["llvm-toolchain-trusty-6.0"], []),
        Package("clang++-6.0", [
            "llvm-toolchain-trusty-6.0",
            "ubuntu-toolchain-r-test"
        ], ["libc++-dev"]),

        Package("gcc-4.9", ["ubuntu-toolchain-r-test"], []),
        Package("g++-4.9", ["ubuntu-toolchain-r-test"], []),
        Package("gcc-5", ["ubuntu-toolchain-r-test"], []),
        Package("g++-5", ["ubuntu-toolchain-r-test"], []),
        Package("gcc-6", ["ubuntu-toolchain-r-test"], []),
        Package("g++-6", ["ubuntu-toolchain-r-test"], []),
        Package("gcc-7", ["ubuntu-toolchain-r-test"], []),
        Package("g++-7", ["ubuntu-toolchain-r-test"], []),
        Package("gcc-8", ["ubuntu-toolchain-r-test"], []),
        Package("g++-8", ["ubuntu-toolchain-r-test"], []),
        Package("libboost-all-dev", [], []),
        Package("libhdf5-dev", [], []),
    ]
    packages = { package.name: package for package in packages }


    flags = {
        "clang++-3.9": "-stdlib=libc++",
        "clang++-4.0": "-stdlib=libc++",
        "clang++-5.0": "-stdlib=libc++",
        "clang++-6.0": "-stdlib=libc++",
    }


    compilers = [
        "clang-3.9",
        "clang++-3.9",
        "clang-4.0",
        "clang++-4.0",
        "clang-5.0",
        "clang++-5.0",
        "clang-6.0",
        "clang++-6.0",
        "gcc-4.9",
        "g++-4.9",
        "gcc-5",
        "g++-5",
        "gcc-6",
        "g++-6",
        "gcc-7",
        "g++-7",
        "gcc-8",
        "g++-8",
    ]
    compilers = { compiler_name: Compiler(compiler_name,
        packages[compiler_name], flags.get(compiler_name, "")) \
            for compiler_name in compilers }


    common_packages = [
        packages["libboost-all-dev"],
        packages["libhdf5-dev"],
    ]


    # These collections are used in the permutation. ---------------------------
    build_types = [
        # "Debug",
        "Release"
    ]


    python_versions = [
        # "2.7",
        # "3.5",
        "3.6",
    ]


    # Tuples with names of C compiler and C++ compiler.
    compiler_families = {
        "gcc": [
                # ("gcc-4.9", "g++-4.9"),
                ("gcc-5", "g++-5"),
                # ("gcc-6", "g++-6"),
                # ("gcc-7", "g++-7"),
                # ("gcc-8", "g++-8"),
            ],
        "clang": [
                # ("clang-3.9", "clang++-3.9"),
                # ("clang-4.0", "clang++-4.0"),
                ("clang-5.0", "clang++-5.0"),
                # ("clang-6.0", "clang++-6.0"),
            ],
    }


    common_environment = {
        # "TRAVIS_PYTHON_VERSION": "2.7.3",
    }


    build_configurations = [

        # Default build (C++ library)
        BuildConfiguration(
            {
            },
            [
            ]
        ),

        # # Default build, including tests
        # BuildConfiguration(
        #     {
        #         "LUE_BUILD_TEST:BOOL": "TRUE",
        #     },
        #     [
        #     ]
        # ),

        # BuildConfiguration(
        #     {
        #         "LUE_BUILD_CXX_API:BOOL": "TRUE",
        #     },
        #     [
        #     ]
        # ),

        # BuildConfiguration(
        #     {
        #         "LUE_BUILD_PYTHON_API:BOOL": "TRUE",
        #     },
        #     [
        #     ]
        # ),

        # BuildConfiguration(
        #     {
        #         "LUE_BUILD_UTILITIES:BOOL": "TRUE",
        #     },
        #     [
        #     ]
        # ),
        # BuildConfiguration(
        #     {
        #         "LUE_BUILD_TEST:BOOL": "TRUE",
        #     },
        #     [
        #     ]
        # ),
        # BuildConfiguration(
        #     {
        #         "LUE_BUILD_CXX_API:BOOL": "TRUE",
        #         "LUE_BUILD_TEST:BOOL": "TRUE",
        #     },
        #     [
        #     ]
        # ),
        # BuildConfiguration(
        #     {
        #         "LUE_BUILD_PYTHON_API:BOOL": "TRUE",
        #         "LUE_BUILD_TEST:BOOL": "TRUE",
        #     },
        #     [
        #     ]
        # ),
        # BuildConfiguration(
        #     {
        #         "LUE_BUILD_UTILITIES:BOOL": "TRUE",
        #         "LUE_BUILD_TEST:BOOL": "TRUE",
        #     },
        #     [
        #     ]
        # ),
    ]


    # Permutate all aspects of a build.
    builds = []

    for build_configuration in build_configurations:
        for compiler_family_name in compiler_families:
            for c_compiler_name, cxx_compiler_name in compiler_families[
                    compiler_family_name]:
                for python_version in python_versions:
                    for build_type in build_types:
                        builds.append(
                            Build(
                                    Compilers(compiler_family_name,
                                        compilers[c_compiler_name],
                                        compilers[cxx_compiler_name]),
                                    python_version,
                                    build_type,
                                    common_packages +
                                        build_configuration.packages +
                                        [compilers[cxx_compiler_name].package],
                                    merge_dicts(common_environment,
                                        build_configuration.environment)
                                )
                            )

    return builds


@devbase.checked_call
def generate_travis_config():

    # Generate a Travis configuration file. There are many combinations of
    # build configurations we want to test and the Travis configuration
    # file doesn't seem to handle our use case well. This means that we
    # must repeat a lot of configuration code. It is better to generate
    # this.


# TODO with unit tests and without unit tests


    configuration = """\
# DO NOT EDIT THIS FILE DIRECTLY!
# This file is generated by a script (generate_travis_config.py).

dist: trusty
sudo: false
language: cpp

matrix:
    include:
        {}


# Install/update dependencies required by the installation of the
# requirements of the project.
# Travis-specific stuff.
before_install:
    # /home/travis/build/pcraster/lue
    - python --version

    # Install conda and some Python packages.
    - wget https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh -O miniconda.sh;
    - bash miniconda.sh -b -p $HOME/miniconda
    - export PATH="$HOME/miniconda/bin:$PATH"
    - hash -r
    - conda config --set always_yes yes --set changeps1 no
    - conda update -q conda
    - conda info -a  # Useful for debugging any issues with conda
    # conda create -q -n test-environment python=$TRAVIS_PYTHON_VERSION numpy
    - conda create -q -n test-environment numpy
    - source activate test-environment
    # - python setup.py install


    # Root of software we installed.
    - mkdir local

    # Locations for temporary files.
    - mkdir tmp tmp/source tmp/build


    # Install a recent version of CMake. ---------------------------------------
    # ~/tmp
    - cd tmp
    - wget --no-check-certificate http://www.cmake.org/files/v3.9/cmake-3.9.1-Linux-x86_64.tar.gz
    # ~/local
    - cd ../local
    - tar zxf ../tmp/cmake-3.9.1-Linux-x86_64.tar.gz
    - export PATH=$PWD/cmake-3.9.1-Linux-x86_64/bin:$PATH
    - cd ..


    # Install additional software not provided by Travis.
    # ~/tmp/source
    - cd tmp/source
    - git clone --recursive https://github.com/geoneric/peacock.git
    # ~/tmp/build
    - cd ../build
    - mkdir peacock
    # ~/tmp/build/peacock
    - cd peacock
    - cmake --version
    - CXX=$TRAVIS_CXX_COMPILER cmake -Dpeacock_prefix=$TRAVIS_BUILD_DIR/local -Dbuild_gdal=true -Dgdal_version=2.0.1 -Dbuild_docopt=true -Ddocopt_version=0.6.2 $TRAVIS_BUILD_DIR/tmp/source/peacock
    - CXX=$TRAVIS_CXX_COMPILER cmake --build . --target all
    # ~
    - cd ../../..


# Install dependencies required by the project, similar to what a user would
# have to do.
install:
    # /home/travis/build/pcraster/lue


# Commands which need to be executed before building the project.
# Travis-specific stuff.
before_script:
    # /home/travis/build/pcraster/lue
    - pip install conan
    - mkdir build
    - cd build
    # When building ourselves use:
    #     -s compiler=gcc compiler.version=7 -s build_type=$TRAVIS_BUILD_TYPE
    - conan install ..
    - cd ..


# Build the project, similar to what a user would have to do.
script:
    # /home/travis/build/pcraster/lue
    - cd build
    - tree -d $TRAVIS_BUILD_DIR/local
    - cmake -DPEACOCK_PREFIX:PATH=$TRAVIS_BUILD_DIR/local $TRAVIS_LUE_CMAKE_ARGUMENTS ..
    - cmake --build . --target all
    - if [[ $TRAVIS_LUE_CMAKE_ARGUMENTS == *"LUE_BUILD_TEST:BOOL=TRUE"* ]]; then cmake --build . --target test; fi


notifications:
    email:
        - k.dejong1@uu.nl
""".format(
        "\n        ".join(["\n        ".join([line for line in str(build).strip().split("\n")]) for build in builds()]))

    print(configuration)


if __name__ == "__main__":
    arguments = docopt.docopt(doc_string)

    function = generate_travis_config
    arguments = ()

    sys.exit(function(*arguments))
