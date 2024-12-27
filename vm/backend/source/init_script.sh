#!/bin/bash

mkdir -p infiles outfiles

#---------redirection setup---------------
touch infiles/no_perms infiles/no_write infiles/no_read infiles/test
echo 'This is a test file !' >infiles/test
chmod 000 infiles/*
chmod +rx ./infiles/no_write
chmod +wx ./infiles/no_read
#-----------------------------------------


#---------pattern matching setup----------
#Sample files for patterm matching
mkdir -p ./data ./pm_test

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
chmod 000 ./pm_test/no_perms
#-----------------------------------------
