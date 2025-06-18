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
        --single-branch lue-${lue_version}
}

function zip_sources() {
    echo "Zip source directory as $tmp_prefix/lue-${lue_version}.tar.bz2 ..."
    tar cjf lue-${lue_version}.tar.bz2 lue-${lue_version}
    openssl sha256 lue-${lue_version}.tar.bz2
    rm -fr lue-$lue_version
}

tmp_prefix="${HOME}/tmp"
parse_command_line $@
clone_sources
zip_sources
