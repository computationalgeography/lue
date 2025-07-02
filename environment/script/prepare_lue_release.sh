#!/usr/bin/env bash
set -eu

function parse_command_line() {
    usage="$(basename $0) lue_fork lue_version git_branch"

    if [[ $# != 3 ]]; then
        echo $usage
        exit 1
    fi

    lue_fork=$1
    lue_version=$2
    git_branch=$3
}

function clone_sources() {
    echo "Clone sources ..."
    git clone \
        --quiet \
        --depth 1 https://github.com/${lue_fork}/lue.git \
        --branch $git_branch \
        --single-branch $clone_pathname
}

function zip_sources() {
    echo "Zip source directory as $zip_pathname ..."

    tar --create --directory $tmp_prefix --bzip2 --file $zip_pathname $(basename $clone_pathname)
    openssl sha256 $zip_pathname
    rm -fr $clone_pathname
}

parse_command_line $@

tmp_prefix="${HOME}/tmp"
clone_pathname="$tmp_prefix/lue-$lue_version"
zip_pathname="$tmp_prefix/lue-${lue_version}.tar.bz2"

clone_sources
zip_sources
