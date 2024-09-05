#!/usr/bin/env bash
set -e
if [[ -z $1 ]]; then
    echo "need a lex file"
    exit 1
fi

flex $1
cc -g -O0 ./lex.yy.c -lfl
