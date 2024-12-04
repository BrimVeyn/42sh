var=10; while true $((--var)) && ! (exit $((var >= 0))); do
	if ! (exit $((var % 2 == 0)) ); then
		echo "value: ${var}";
	fi
done

var=10; while true $((--var)) && ! (exit $((var >= 0))); do echo ${var}; done
var=5; while true $((--var)) && (exit $((var < 0))); do echo "Countdown: ${var}"; done
var=0; while true $((++var)) && (exit $((var > 10))); do echo "Counting up: ${var}"; done
var=15; while true $((--var)) && ! (exit $((var % 3 != 0))); do echo "Not divisible by 3: ${var}"; done
var=0; while true $((++var)) && ! (exit $((var % 5 != 0))); do echo "Skipping multiples of 5: ${var}"; done

var=0; while true $((++var)) && (exit $((var > 10))); do
	if ! (exit $((var % 2))); then
		echo "${var} is even";
	fi;
done

i=3; while true $((--i)) && (exit $((i < 0))); do
	j=3; while true $((--j)) && (exit $((j < 0))); do
		echo "i=${i}, j=${j}";
	done;
done

var=10; while true $((--var)) && (exit $((var < 0))); do echo echo "Value: ${var}"; done

var=10; while true $((--var)) && (exit $((var < 0))); do echo $(echo "Value: ${var}"); done
var=5; while true $((++var)) && (exit $((var > 15))); do 
	echo "$(echo ${var} | tr '0-9' 'a-j')"; 
done

var=20; while true $((--var)) && ! (exit $((var > 0))); do
	(exit $((var % 2 == 0))) || echo "${var} is odd"
done
var=50; while true $((--var)) && (exit $((var < 0))); do
	if ! (exit $((var % 3 == 0))); then echo "${var} is divisible by 3"; fi
done

while true $((0)) && (exit $((1))); do echo "Shouldn't run"; done
while true $((1)) && ! (exit $((0))); do echo "Runs once"; break; done
var=-5; while true $((++var)) && ! (exit $((var > 0))); do echo "$var"; done
