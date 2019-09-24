#!/usr/bin/env bash
# make obj-intel64/addrtrace.so
set -e

command="../../../../pin -t ../obj-intel64/addrtrace.so -- ../obj/prog$1 8"
eval ${command}
