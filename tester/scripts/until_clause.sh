var=10; until true $((--var)) || (exit $((var < 0))); do
	if ! (exit $((var % 2 == 0)) ); then
		echo "Odd value: ${var}";
	fi
done

var=5; until true $((--var)) || (exit $((var < 0))); do echo "Countdown: ${var}"; done

var=0; until true $((++var)) || (exit $((var > 10))); do echo "Counting up: ${var}"; done

var=15; until true $((--var)) || (exit $((var % 3 == 0))); do echo "Not divisible by 3: ${var}"; done

var=0; until true $((++var)) || (exit $((var % 5 == 0))); do echo "Skipping multiples of 5: ${var}"; done

var=0; until true $((++var)) || (exit $((var > 10))); do
	if ! (exit $((var % 2))); then
		echo "${var} is even";
	fi;
done

i=3; until true $((--i)) || (exit $((i < 0))); do
	j=3; until true $((--j)) || (exit $((j < 0))); do
		echo "i=${i}, j=${j}";
	done;
done

var=10; until true $((--var)) || (exit $((var < 0))); do echo echo "Value: ${var}"; done

var=10; until true $((--var)) || (exit $((var < 0))); do echo $(echo "Value: ${var}"); done

var=5; until true $((++var)) || (exit $((var > 15))); do
	echo "$(echo ${var} | tr '0-9' 'a-j')";
done

var=20; until true $((--var)) || (exit $((var < 0))); do
	(exit $((var % 2 != 0))) && echo "${var} is even";
done

var=50; until true $((--var)) || (exit $((var < 0))); do
	if ! (exit $((var % 3 == 0))); then echo "${var} is divisible by 3"; fi
done

until true $((0)) || (exit $((1))); do echo "Shouldn't run"; done

until true $((1)) || ! (exit $((0))); do echo "Runs once"; break; done

var=-5; until true $((++var)) || (exit $((var > 0))); do echo "$var"; done

var=0; until true $((++var)) || (exit $((var > 20))); do
	if ! (exit $((var % 4))); then
		echo "${var} is divisible by 4";
	fi;
done
