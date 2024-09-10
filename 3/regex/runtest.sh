#!/usr/bin/env bash
set -e

valgrind \
	--tool=memcheck \
	--leak-check=full \
	--show-leak-kinds=all \
	./bin/unit_test

