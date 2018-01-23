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
    # Don't build any of the support libraries...
    build_boost=false
    build_docopt=false
    build_gdal=false
    build_google_benchmark=false
    build_nlohmann_json=false
    build_pybind11=false

    # ...except for these machines
    hostname=`hostname`
    build_docopt=true
    build_google_benchmark=true
    build_nlohmann_json=true
    build_pybind11=true

    if [ $hostname == "sonic.geo.uu.nl" ]; then
        build_boost=true
        build_gdal=true
    elif [ $hostname == "gransasso" ]; then
        build_gdal=true
    fi


    options+=("-Dpeacock_download_dir=$download_dir")
    options+=("-Dpeacock_prefix=$prefix")
    # options+=("-DCMAKE_VERBOSE_MAKEFILE=ON")


    if [ "$build_boost" = true ]; then
        options+=("-Dbuild_boost=true")
        options+=("-Dboost_version=1.63.0")

        options+=("-Dboost_build_boost_filesystem=true")
        options+=("-Dboost_build_boost_program_options=true")
        options+=("-Dboost_build_boost_system=true")
        options+=("-Dboost_build_boost_test=true")
    fi


    if [ "$build_pybind11" = true ]; then
        options+=("-Dbuild_pybind11=true")
        options+=("-Dpybind11_version=2.2.1")
    fi


    if [ "$build_docopt" = true ]; then
        options+=("-Dbuild_docopt=true")
        options+=("-Ddocopt_version=0.6.2")
    fi


    if [ "$build_gdal" = true ]; then
        options+=("-Dbuild_gdal=true")
        options+=("-Dgdal_version=2.0.1")
    fi


    if [ "$build_nlohmann_json" = true ]; then
        options+=("-Dbuild_nlohmann_json=true")
        options+=("-Dnlohmann_json_version=3.0.1")
    fi


    cmake "${options[@]}" $source
    cmake --build . --target all
}


parse_commandline $*
build_peacock
