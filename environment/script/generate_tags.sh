#!/usr/bin/env bash
cd $LUE
ctags --recurse --exclude=source/lue-0.0.0 --exclude=source/lue-0.1.0 source
