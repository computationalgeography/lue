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
    hostname=`hostname`

    if [[ $hostname != "sonic.geo.uu.nl" ]]; then
        skip_build_boost=1
    fi

    if [[ $hostname == "triklav" ]]; then
        skip_build_gdal=1
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


    # Boost.
    if [ ! "$skip_build_boost" ]; then
        options+=("-Dbuild_boost=true")
        options+=("-Dboost_version=1.57.0")
        options+=("-Dboost_build_boost_filesystem=true")
        options+=("-Dboost_build_boost_program_options=true")
        options+=("-Dboost_build_boost_system=true")
        options+=("-Dboost_build_boost_test=true")
    fi


    # pybind11
    options+=("-Dbuild_pybind11=true")
    options+=("-Dpybind11_version=1.8.0")


    # Docopt.
    options+=("-Dbuild_docopt=true")
    options+=("-Ddocopt_version=0.6.1")


    # GDAL.
    if [ ! "$skip_build_gdal" ]; then
        options+=("-Dbuild_gdal=true")
        options+=("-Dgdal_version=2.0.1")
    fi


    cmake "${options[@]}" $source
    cmake --build . --target all
}


parse_commandline $*
build_peacock
