#!/usr/bin/env bash
set -e

dirname="$( cd "$(dirname "$0")" ; pwd -P )"

benchmark_kind=weak_scaling
benchmark_name=copy

source $dirname/common.sh
