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

# Création des répertoires
mkdir -p ./input_files
mkdir -p ./output_files

# Création des fichiers dans input_files
echo -e "Hello World!\n!dlroW olleH" > ./input_files/normal
touch ./input_files/no_perms && chmod 0000 ./input_files/no_perms
touch ./input_files/no_exec && chmod 0644 ./input_files/no_exec
touch ./input_files/no_write && chmod 0444 ./input_files/no_write
touch ./input_files/no_read && chmod 0222 ./input_files/no_read

# Création des fichiers dans output_files
echo -e "Hello World!\n!dlroW olleH" > ./output_files/normal
touch ./output_files/no_perms && chmod 0000 ./output_files/no_perms
touch ./output_files/no_exec && chmod 0644 ./output_files/no_exec
touch ./output_files/no_write && chmod 0444 ./output_files/no_write
touch ./output_files/no_read && chmod 0222 ./output_files/no_read
 
chmod 0 ./pm_test/no_perms
