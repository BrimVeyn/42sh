#!/bin/bash

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

clean_cd () {
	chmod -R 777 test_cd test_dir testdir outfiles
	rm -rf test_cd test_dir testdir outfiles
}

test_files=(
	"src/basic"
	"src/syntax"
	"src/quotes_1"
	"src/quotes_2"
	"src/redirections"
	"src/subshell_1"
	"src/subshell_2"
	"src/subshell_3"
	"src/command_group"
	"src/command_sub_1"
	"src/command_sub_2"
	"src/parameter_expansion"
	"src/arithmetic_expansion_1"
	"src/arithmetic_expansion_2"
	"src/arithmetic_expansion_3"
	"src/arithmetic_expansion_4"
	"src/tilde_expansion"
	"src/builtin_export"
	"src/builtin_cd"
	"src/builtin_exit"
	"src/builtin_echo"
	"src/mixed_1"
	"src/mixed_2"
	"src/mixed_3"
)

SH42_PATH="./42sh"
BASH_PATH="/usr/bin/bash"
ECHO_PATH="/usr/bin/echo"
OUTPUT_PATH="./data/"

mkdir -p ${OUTPUT_PATH}{input_logs,output_logs,error_logs}

start_tests ()
{
	{
		#put json's header
		echo -e '{'
		echo -e "\t\"categories\": ["

		testId=1

		#read every sourced files
		for i in "${!test_files[@]}"; do

			file=${test_files[i]}

			# get categorie name src/ex --> ex
			category=$(basename ${file})
			passed="0"

			#start category
			echo -e "\t\t{"
			echo -e "\t\t\t\"category_name\": \"${category}\","
			echo -e "\t\t\t\"tests\": ["

			mapfile -t lines < "${file}"

			#read unit tests line by line
			for j in "${!lines[@]}"; do

				line=${lines[j]}

				SH42_OUTPUT=$( ${ECHO_PATH} -e "${line}" | ${SH42_PATH} 1>${OUTPUT_PATH}output_logs/42sh_${testId} 2>${OUTPUT_PATH}error_logs/42sh_${testId})
				SH42_EXITNO=${?}
				SH42_OUTPUT=${SH42_OUTPUT:-"None"}

				BASH_OUTPUT=$( ${ECHO_PATH} -e "${line}" | ${BASH_PATH} 1>${OUTPUT_PATH}output_logs/bash_${testId} 2>${OUTPUT_PATH}error_logs/bash_${testId})
				BASH_EXITNO=${?}
				BASH_OUTPUT=${BASH_OUTPUT:-"None"}

				#output formatted json test recap
				echo ${line} 1>${OUTPUT_PATH}input_logs/input_${testId}

				echo -ne "\t\t\t\t{\n" \
				"\t\t\t\t\t\"id\": \"${testId}\",\n" \
				"\t\t\t\t\t\"input\": \"${OUTPUT_PATH}input_logs/input_${testId}\",\n" \
				"\t\t\t\t\t\"42sh_output\": \"${OUTPUT_PATH}output_logs/42sh_${testId}\",\n" \
				"\t\t\t\t\t\"bash_output\": \"${OUTPUT_PATH}output_logs/bash_${testId}\",\n" \
				"\t\t\t\t\t\"42sh_error\": \"${OUTPUT_PATH}error_logs/42sh_${testId}\",\n" \
				"\t\t\t\t\t\"bash_error\": \"${OUTPUT_PATH}error_logs/bash_${testId}\",\n" \
				"\t\t\t\t\t\"42sh_exit_code\": \"${SH42_EXITNO}\",\n" \
				"\t\t\t\t\t\"bash_exit_code\": \"${BASH_EXITNO}\",\n"

				validate="1"

				if cmp -s "${OUTPUT_PATH}output_logs/bash_${testId}" "${OUTPUT_PATH}output_logs/42sh_${testId}"; then
					echo -e "\t\t\t\t\t\"output_ok\": \"true\","
				else
					echo -e "\t\t\t\t\t\"output_ok\": \"false\","
					validate="0"
				fi

				if cmp -s "${OUTPUT_PATH}error_logs/bash_${testId}" "${OUTPUT_PATH}error_logs/42sh_${testId}"; then
					echo -e "\t\t\t\t\t\"error_ok\": \"true\","
				else
					echo -e "\t\t\t\t\t\"error_ok\": \"false\","
				fi

				if [[ "${SH42_EXITNO}" = "${BASH_EXITNO}" ]]; then
					echo -e "\t\t\t\t\t\"exit_ok\": \"true\""
				else
					echo -e "\t\t\t\t\t\"exit_ok\": \"false\""
					validate="0"
				fi

				if [[ "${validate}" = "1" ]]; then
					((passed++))
				fi

				echo -ne "\t\t\t\t}";

				if ((j < ${#lines[@]} - 1 )); then
					echo -ne ","
				fi

				echo -ne "\n"

				((testId++))

			done <${file}


			#end of category
			echo -e "\t\t\t],"
			echo -e "\t\t\t\"passed_tests\": \"${passed}\""
			echo -ne "\t\t}"

			if ((i < ${#test_files[@]} - 1)); then
				echo -ne ","
			fi
			
			echo -ne "\n"

		done 

		#end json file
		echo -e "\t]"
		echo -e "}"

	} >${OUTPUT_PATH}log.json
}

setup_cd_test_environment
setup_redirection_test_environment

start_tests

clean_redirection
clean_cd
