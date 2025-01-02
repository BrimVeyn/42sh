#!/bin/bash

source_path="./tester_src"
declare -a source_files
mapfile -t source_files < <(ls ${source_path})

for file in ${source_files[@]}; do
	rm -rf ${file}*
done

test_files=(
	"./pm_test"
	"./test_dir"
	"./testdir"
	"./outfiles"
	"./infiles"
)

chmod -R 777 ${test_files[@]} 2>/dev/null
rm -rf ${test_files[@]} 2>/dev/null
