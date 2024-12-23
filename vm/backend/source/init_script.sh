#!/bin/bash

mkdir -p ./data ./pm_test

#Sample files for patterm matching
touch ./pm_test/{a..k}{0..6}
touch ./pm_test/{a..d}dd
touch ./pm_test/{a..d}e
touch ./pm_test/{A..D}e
touch ./pm_test/{g..m}kk
touch ./pm_test/{1..5}{a..d}
touch ./pm_test/\[test
touch ./pm_test/\~test
touch ./pm_test/\*\*\*test
touch ./pm_test/file{1..5}.txt
touch ./pm_test/"file*"
touch ./pm_test/"file with*"
touch ./pm_test/{a..c}0{A..B}e
touch ./pm_test/file\ with\ spaces

#Dir without permissions
mkdir ./pm_test/no_perms
touch ./pm_test/no_perms/{1..9}test

chmod 0 ./pm_test/no_perms
