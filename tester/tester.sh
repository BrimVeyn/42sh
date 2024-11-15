#!/bin/bash
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

start_tests ()
{
	mkdir -p ./outfiles
	mkdir -p ./42sh_outfiles
	mkdir -p ./42sh_error
	mkdir -p ./bash_outfiles
	mkdir -p ./bash_error
	setup_cd_test_environment

	chmod 000 infiles/no_perms

	i=0
	ok=0
	buffer_error="";
	max_line_len=60;

	for file in "${test_lists[@]}"
	do
		equals_len=$(( (max_line_len / 2) - (${#file} / 2) ))
		printf "${SANDY_BROWN}" && printf "%${equals_len}s" " " | tr ' ' '=';
		printf "${CINNABAR}${file^^}";
		printf "${SANDY_BROWN}" && printf "%${equals_len}s" " " | tr ' ' '='; printf "\n"
		line_len=0;
		while IFS= read -r line
		do
			
			((i++))

			rm -rf ./outfiles/*
			rm -rf ./42sh_outfiles/*

			EXEC_OUTPUT=$(echo -e "$line" | $EXEC_PATH 2>./42sh_error/e)
			EXEC_EXITNO=${?}
			EXEC_OUTPUT=${EXEC_OUTPUT:-"None"}
			EXEC_ERROR=$(sed 's/.*://' ./42sh_error/e || echo "null")
			EXEC_COPY=$(cp ./outfiles/* ./42sh_outfiles &>/dev/null || echo "There is no file to copy")

			rm -rf ./outfiles/*
			rm -rf ./bash_outfiles/*

			BASH_OUTPUT=$(echo -e "$line" | bash 2>./bash_error/e)
			BASH_OUTPUT=${BASH_OUTPUT:-"None"}
			BASH_COPY=$(cp ./outfiles/* ./bash_outfiles &>/dev/null || echo "There is no file to copy")
			EXEC_ERROR=$(sed 's/.*://' ./bash_error/e || echo "null")
			BASH_EXITNO=$(echo ${?})

			FILE_DIFF=$(diff -q ./42sh_outfiles ./bash_outfiles)

			
			if [[ "$EXEC_OUTPUT" == "$BASH_OUTPUT" && $FILE_DIFF == "" && "$EXEC_EXITNO" == "$BASH_EXITNO" && "$EXEC_COPY" == "$BASH_COPY" ]]; then
				if [[ "$EXEC_ERROR" != "$BASH_ERROR" ]]; then
					printf "⚠️ "
				else
					printf "✅${RESET}"
				fi
				((ok++)); ((line_len++));
				if ((line_len >= max_line_len / 2)); then printf "\n"; line_len=0; fi
				continue
			else
				printf "❌"
			fi
			((line_len++));
			if ((line_len >= max_line_len / 2)); then printf "\n"; line_len=0; fi

			buffer_error+=$(printf "\n${BLUE}${BOLD}Test %4s: ${YELLOW}${line}\n${RESET}" $i);
			if [[ "$EXEC_OUTPUT" != "$BASH_OUTPUT" ]]; then
				buffer_error+=$(printf "${BITTERSWEET}${BOLD}STDOUT: %.100s\n${RESET}" "$EXEC_OUTPUT");
				buffer_error+=$(printf "${SGBUS}${BOLD}STDOUT: %.100s\n${RESET}" "$BASH_OUTPUT");
			fi
			if [[ "$EXEC_ERROR" != "$BASH_ERROR" && debug -eq 1 ]]; then
				buffer_error+=$(printf "${BITTERSWEET}${BOLD}STDERR: %s\n${RESET}" "$EXEC_ERROR");
				buffer_error+=$(printf "${SGBUS}${BOLD}STDERR: %s\n${RESET}" "$BASH_ERROR");
			fi
			if [[ "$EXEC_EXITNO" != "$BASH_EXITNO" ]]; then
				buffer_error+=$(printf "${BITTERSWEET}${BOLD}Exitno: ${SELECTIVE_YELLOW}%s\n${RESET}" "$EXEC_EXITNO");
				buffer_error+=$(printf "${SGBUS}${BOLD}Exitno: ${SELECTIVE_YELLOW}%s\n${RESET}" "$BASH_EXITNO");
			fi
			if [[ "$EXEC_COPY" != "$BASH_COPY" ]]; then
				buffer_error+=$(printf "${BITTERSWEET}${BOLD}File copy: %s\n${RESET}" "$EXEC_COPY");
				buffer_error+=$(printf "${SGBUS}${BOLD}File copy: %s\n${RESET}" "$BASH_COPY");
			fi
			if [[ "$FILE_DIFF" ]];then
				buffer_error+=$(echo $FILE_DIFF)
				buffer_error+="\n${BITTERSWEET}${BOLD}Files:${RESET}\n";
				buffer_error+=$({ cat 42sh_outfiles/* | head -n 5 ;} 2>/dev/null);
				buffer_error+="\n${SGBUS}${BOLD}Files:${RESET}\n";
				buffer_error+=$({ cat bash_outfiles/* | head -n 5 ;} 2>/dev/null);
			fi

			printf "$RESET"
		done < "$file"
		printf "\n${SANDY_BROWN}" && printf "%${max_line_len}s" " " | tr ' ' '='; printf "\n"
	done

	echo -e "${buffer_error}"
	printf "${GREEN}${ok}${RESET} / ${GREEN}${i}${RESET} test(s) passed !\n"

	if [[ "${i}" == "${ok}" ]]; then
		printf "${GREEN}Congrats !${RESET}\n";
	fi
}

test_lists=(
	"src/test"
	"src/redirections"
	"src/syntax"
	"src/expand"
	"src/subshell"
	"src/command_sub"
	"src/command_group"
	"src/arithmetic_expansion"
	"src/builtin_export"
	"src/builtin_cd"
	"src/mixed"
)

PS3="Enter a number to run associated tests, * for all: "
select option in redirections syntax expand subshell command_sub command_group arithmetic_expansion builtin_export builtin_cd mixed
do
    case $option in
        redirections)
			test_lists=("src/redirections");
			start_tests;
            ;;
        syntax)
			test_lists=("src/syntax");
			start_tests;
            ;;
        expand)
			test_lists=("src/expand");
			start_tests;
            ;;
        subshell)
			test_lists=("src/subshell");
			start_tests;
            ;;
        command_sub)
			test_lists=("src/command_sub");
			start_tests;
            ;;
        command_group)
			test_lists=("src/command_group");
			start_tests;
            ;;
        arithmetic_expansion)
			test_lists=("src/arithmetic_expansion");
			start_tests;
            ;;
        builtin_export)
			test_lists=("src/builtin_export");
			start_tests;
            ;;
		builtin_cd)
			test_lists=("src/builtin_cd");
			start_tests;
			chmod 777 test_cd/protected
			rm -rf test_cd;
            ;;
        mixed)
			test_lists=("src/mixed");
			start_tests;
            ;;
        *)
			start_tests;
            ;;
    esac
done

rm -rf ./outfiles 2>/dev/null
rm -rf ./42sh_outfiles 2>/dev/null
rm -rf ./bash_outfiles 2>/dev/null
chmod 777 infiles/no_perms
