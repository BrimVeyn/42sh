var=10000; while true $((--var)) && ! (exit $((var >= 0))); do
	if ! (exit $((var % 2 == 0)) ); then
		echo "value: ${var}";
	fi
done
