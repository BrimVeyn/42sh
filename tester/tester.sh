#!/bin/bash
EXEC_PATH="../42sh"
# PROMPT="echo -"

if [[ "${1}" == "-d" ]]; then
	debug=1;
	shift 1;
fi

if [[ $1 ]]; then
	test_lists=(
		$1
	)
else
	test_lists=(
		"test"
		"syntax"
		"expand"
		"subshell"
		"command_sub"
		"command_group"
		"arithmetic_expansion"
		"builtin_export"
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
CINNABAR="\033[0;38;2;244;68;46m"
SANDY_BROWN="\033[0;38;2;252;158;79m"

mkdir -p ./outfiles
mkdir -p ./42sh_outfiles
mkdir -p ./42sh_error
mkdir -p ./bash_outfiles
mkdir -p ./bash_error

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
			buffer_error+=$(printf "42sh output: %.100s\n${RESET}" "$EXEC_OUTPUT");
			buffer_error+=$(printf "bash output: %.100s\n${RESET}" "$BASH_OUTPUT");
		fi
		if [[ "$EXEC_ERROR" != "$BASH_ERROR" && debug -eq 1 ]]; then
			buffer_error+=$(printf "42sh error: %s\n${RESET}" "$EXEC_ERROR");
			buffer_error+=$(printf "bash error: %s\n${RESET}" "$BASH_ERROR");
		fi
		if [[ "$EXEC_EXITNO" != "$BASH_EXITNO" ]]; then
			buffer_error+=$(printf "42sh exitno: %s\n${RESET}" "$EXEC_EXITNO");
			buffer_error+=$(printf "bash exitno: %s\n${RESET}" "$BASH_EXITNO");
		fi
		if [[ "$EXEC_COPY" != "$BASH_COPY" ]]; then
			buffer_error+=$(printf "42sh file copy: %s\n${RESET}" "$EXEC_COPY");
			buffer_error+=$(printf "bash file copy: %s\n${RESET}" "$BASH_COPY");
		fi
		if [[ "$FILE_DIFF" ]];then
			echo $FILE_DIFF
			printf "42sh files:\n"
			cat 42sh_outfiles/*
			printf "\nbash files:\n"
			cat bash_outfiles/*
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

rm -rf ./outfiles 2>/dev/null
rm -rf ./42sh_outfiles 2>/dev/null
rm -rf ./bash_outfiles 2>/dev/null
chmod 777 infiles/no_perms
