#!/bin/bash

quiet=""
if [[ "${1}" == "-q" ]]; then
	quiet="--quiet"
	shift
fi

valgrind ${quiet} --show-leak-kinds=all --leak-check=full --track-origins=yes --trace-children=yes --track-fds=all --suppressions=readline_suppression.supp ${1}

