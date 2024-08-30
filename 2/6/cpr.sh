#!/usr/bin/env bash

set -e

if [[ -z $1 ]]; then
    echo '请输入文件'
    exit 1
fi

g++ $1 -std=c++23 -g
