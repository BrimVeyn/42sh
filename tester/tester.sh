#!/bin/bash

EXEC_PATH="../42sh"
# PROMPT="echo -"

if [[ $1 ]]; then
	test_lists=(
		$1
	)
else
	test_lists=(
		"test"
		"syntax"
	)
fi

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

mkdir ./outfiles
mkdir ./42sh_outfiles
mkdir ./bash_outfiles

i=0
ok=0

for file in "${test_lists[@]}"
do
	while IFS= read -r line
	do
		((i++))

		rm -rf ./outfiles/*
		rm -rf ./42sh_outfiles/*

		EXEC_OUTPUT=$(echo -e "$line" | $EXEC_PATH 2>/dev/null | $REMOVE_COLORS | $REMOVE_EXIT)
		EXEC_ERROR=$(echo -e "$line" | $EXEC_PATH 2>&1 1>/dev/null | $REMOVE_COLORS | $REMOVE_EXIT | rev | cut -d':' -f1 | rev)
		EXEC_EXITNO=$(echo -e "$line\nexit\n" | $EXEC_PATH 2>/dev/null | $REMOVE_COLORS | grep -o '[0-9]*$')
		cp ./outfiles/* ./42sh_outfiles &>/dev/null

		rm -rf ./outfiles/*
		rm -rf ./bash_outfiles/*
		BASH_OUTPUT=$(echo -e "$line" | bash 2>/dev/null)
		BASH_ERROR=$(echo -e "$line" | bash 2>&1 1>/dev/null | rev | cut -d':' -f1 | rev)
		BASH_EXITNO=$(echo ${?})
		cp ./outfiles/* ./bash_outfiles &>/dev/null

		FILE_DIFF=$(diff -q ./42sh_outfiles ./bash_outfiles)

		printf "$BLUE$BOLD Test %3s:" $i
		
		if [[ "$EXEC_OUTPUT" == "$BASH_OUTPUT" && !$FILE_DIFF ]]; then
			printf "  ✅"
			if [[ "$EXEC_ERROR" != "$BASH_ERROR" ]]; then
				printf "⚠️ "
			fi
			((ok++))
		else
			printf "  ❌  "
		fi
		printf "$RESET$GREY%s$RESET \n" "$line"

		if [[ "$EXEC_OUTPUT" != "$BASH_OUTPUT" ]]; then
			printf "42sh output: %s\n" "$EXEC_OUTPUT"
			printf "bash output: %s\n" "$BASH_OUTPUT"
		fi
		if [[ "$EXEC_ERROR" != "$BASH_ERROR" ]]; then
			printf "42sh error: %s\n" "$EXEC_ERROR"
			printf "bash error: %s\n" "$BASH_ERROR"
		fi
		# if [[ "$EXEC_EXITNO" != "$BASH_EXITNO" ]]; then
		# 	printf "42sh exitno: %s\n" "$EXEC_EXITNO"
		# 	printf "bash exitno: %s\n" "$BASH_EXITNO"
		# fi
		if [[ "$FILE_DIFF" ]];then
			echo $FILE_DIFF
			cat 42sh_outfiles/*
			cat bash_outfiles/*
		fi

		printf "$RESET"
	done < "$file"
done

printf "${GREEN}${ok}${RESET} / ${GREEN}${i}${RESET} test(s) passed !\n"

if [[ "${i}" == "${ok}" ]]; then
	printf "${GREEN}Congrats !${RESET}\n";
fi

rm -rf ./outfiles
rm -rf ./42sh_outfiles
rm -rf ./bash_outfiles
