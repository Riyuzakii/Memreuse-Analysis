#!/usr/bin/env bash
# make obj-intel64/addrtrace.so
set -e

case $1 in
    1) command="../../../../pin -t ../obj-intel64/addrtrace.so -- ../obj/prog$2 8"
        ;;
    2) command="cd .. && ./obj/part2"
        ;;
    3) command="cd .. && ./obj/part3"
        ;;
    4) command="cd .. && ./obj/part4"
        ;;
    *) echo "Part number $1 not found"
        ;;
esac
eval ${command}
