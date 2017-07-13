#!/usr/bin/env bash
set -e


function print_usage()
{
    echo -e "\
usage: $0 [-h] <download_dir> <prefix> <source>

-h              Show (this) usage information.

download_dir    Directory to store downloaded files.
prefix          Directory to install the resulting files.
source          Directory of Peacock sources."
}


function parse_commandline()
{
    while getopts h option; do
        case $option in
            h) print_usage; exit 0;;
            *) print_usage; exit 2;;
        esac
    done
    shift $((OPTIND-1))

    if [ $# -ne 3 ]; then
        print_usage
        exit 2
    fi

    download_dir=$1
    prefix=$2
    source=$3
}


function build_peacock()
{
    # Don't build any support libraries...
    skip_build_boost=1
    skip_build_docopt=1
    skip_build_gdal=1
    skip_build_pybind11=1

    # ...except for these machines
    hostname=`hostname`
    skip_build_docopt=0
    skip_build_pybind11=0

    if [[ $hostname == "sonic.geo.uu.nl" ]]; then
        skip_build_boost=0
    fi

    if [[ $hostname != "triklav.soliscom.uu.nl" ]]; then
        skip_build_gdal=0
    fi


    if [[ $OSTYPE == "cygwin" ]]; then
        options+=("-GUnix Makefiles")
        options+=("-DCMAKE_MAKE_PROGRAM=mingw32-make")
    fi

    if [[ $OSTYPE == "linux-gnu" ]] && [[ "$CC" == *mingw* ]]; then
        # Cross-compiling, need toolchain file.
        options+=("-DCMAKE_TOOLCHAIN_FILE=$source/cmake/toolchain/mingw-linux.cmake")
    fi

    options+=("-Dpeacock_download_dir=$download_dir")
    options+=("-Dpeacock_prefix=$prefix")
    options+=("-DCMAKE_VERBOSE_MAKEFILE=ON")


    if [ ! "$skip_build_boost" ]; then
        options+=("-Dbuild_boost=true")
        options+=("-Dboost_version=1.57.0")
        options+=("-Dboost_build_boost_filesystem=true")
        options+=("-Dboost_build_boost_program_options=true")
        options+=("-Dboost_build_boost_system=true")
        options+=("-Dboost_build_boost_test=true")
    fi


    if [ ! "$skip_build_pybind11" ]; then
        options+=("-Dbuild_pybind11=true")
        options+=("-Dpybind11_version=2.1.0")
    fi


    if [ ! "$skip_build_docopt" ]; then
        options+=("-Dbuild_docopt=true")
        options+=("-Ddocopt_version=0.6.1")
    fi


    if [ ! "$skip_build_gdal" ]; then
        options+=("-Dbuild_gdal=true")
        options+=("-Dgdal_version=2.0.1")
    fi


    # nlohmann JSON.
    options+=("-Dbuild_nlohmann_json=true")
    options+=("-Dnlohmann_json_version=2.1.1")


    cmake "${options[@]}" $source
    cmake --build . --target all
}


parse_commandline $*
build_peacock
