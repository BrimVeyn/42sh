var=10
start_time=$(date +%s%N) # Initial timestamp in nanoseconds

while true $((--var)) && ! (exit $((var >= 0))); do
    end_time=$(date +%s%N) # Timestamp at the current iteration
    elapsed=$((end_time - start_time)) # Calculate elapsed time in nanoseconds
	echo "value: ${var}, elapsed time: $((elapsed / 1000000)) ms"
    start_time=${end_time} # Update start_time for the next iteration
done
