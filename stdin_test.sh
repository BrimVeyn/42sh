func () {
	for arg in ${@}; do
		echo ${arg};
	done
}

func un deux trois

print_all_args () { 
	for arg in ${@}; do 
		echo "${arg}"; 
	done; 
}

print_all_args "arg1" "arg2" "arg3"

