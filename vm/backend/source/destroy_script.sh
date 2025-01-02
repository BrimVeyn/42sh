#!/bin/bash

test_files=(
	"./pm_test"
	"./test_dir"
	"./testdir"
	"./outfiles"
	"./infiles"
)

chmod -R 777 ${test_files[@]} 2>/dev/null

rm -rf ${test_files[@]} 2>/dev/null
