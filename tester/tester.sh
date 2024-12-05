#!/usr/bin/bash

EXEC_PATH="../42sh"

REMOVE_COLORS="sed s/\x1B\[[0-9;]\{1,\}[A-Za-z]//g"
REMOVE_EXIT="grep -v ^exit$"

BOLD="\e[1m"
YELLOW="\033[0;33m"
GREY="\033[38;5;244m"
PURPLE="\033[0;35m"
GREEN="\033[92m"
BLUE="\033[0;36m"
RED="\e[0;31m"
RESET="\033[0m"
CINNABAR="\033[0;38;2;244;68;46m"
SANDY_BROWN="\033[0;38;2;252;158;79m"
BITTERSWEET="\033[0;38;2;255;81;84m"
SGBUS="\033[0;38;2;124;223;100m"
SELECTIVE_YELLOW="\033[0;38;2;255;188;31m"

setup_cd_test_environment() {
    mkdir -p test_cd
    cd test_cd || exit

    mkdir -p dir1 dir2 dir3/subdir

    mkdir -p protected
    touch protected/file
    chmod -rwx protected

    ln -s dir1 link_to_dir1
    ln -s dir2 link_to_dir2
    ln -s dir3/subdir link_to_subdir
    ln -s ../dir2 dir3/link_to_parent_dir2

    mkdir -p symlink_chain
    ln -s ../dir1 symlink_chain/step1
    ln -s ../dir2 symlink_chain/step2
    ln -s ../dir3 symlink_chain/step3

    cd ..
}

setup_redirection_test_environment() {
	mkdir -p infiles
	mkdir -p outfiles
	touch infiles/no_perms
	touch infiles/no_exec
	touch infiles/no_write
	touch infiles/no_read

	echo "salut voici un fichier test" > infiles/test
	cat infiles/test > infiles/no_read
	cat infiles/test > infiles/no_write

	chmod 000 infiles/no_perms
	chmod -x infiles/no_exec
	chmod -r infiles/no_read
	chmod -w infiles/no_write
	touch outfiles/no_perms
	touch outfiles/no_exec
	touch outfiles/no_write
	touch outfiles/no_read

	echo "salut voici un fichier test" > outfiles/test
	cat outfiles/test > outfiles/no_read
	cat outfiles/test > outfiles/no_write

	chmod 000 outfiles/no_perms
	chmod -x outfiles/no_exec
	chmod -r outfiles/no_read
	chmod -w outfiles/no_write
}

clean_redirection() {
	chmod 777 infiles/*
	rm -rf infiles
}


start_test () {

	mkdir -p ./outfiles
	mkdir -p ./42sh_outfiles
	mkdir -p ./42sh_error
	mkdir -p ./bash_outfiles
	mkdir -p ./bash_error

	it=0
	test_passed=0
	buffer_error="";
	max_line_len=60;


}
