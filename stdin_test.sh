var=255;

while true $((var--)) && ! (exit ${var}); do
	sleep 0.01;
	echo ${var};
done
