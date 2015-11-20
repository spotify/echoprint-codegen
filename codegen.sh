#!/bin/sh
#
# To make it easy to use with GNU Parallel, e.g.,
#   parallel codegen.sh ::: *.mp3
#
echoprint-codegen "$1" > "$1.json"
