func_one () {
	echo ${0},${1},${2}
}

# func_one test hey;
# func_one nathan warren;

echo ${1},${2};

func_one wooooo hey

echo ${1},${2};

func_one un deux

echo ${1},${2};

