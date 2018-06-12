#!/usr/bin/env bash
set -e

version="0.19"

rm -f *.{css,js}
wget \
    --recursive \
    --no-host-directories \
    --no-parent \
    --no-directories \
    --reject index.html \
    https://casual-effects.com/markdeep/$version/
