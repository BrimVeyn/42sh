for var in 1 2 3 4 5; do echo "Value: ${var}"; done
for var in $(seq 5 -1 1); do echo "Reverse: ${var}"; done
for var in a b c d e; do echo "Letter: ${var}"; done
for var in "" ""; do echo "Empty string: ${var}"; done
for var in "one word" "two words" ""; do echo "Var: '${var}'"; done

for var in $(seq -5 5); do
	(exit $((var >= 0))) || echo "Negative: ${var}"
done

for i in $(seq 1 3); do
	for j in $(seq 1 3); do
		echo "i=${i}, j=${j}"
	done
done

for var in $(seq 0 5); do echo "$(echo "${var} squared is $((var * var))")"; done
for var in $(echo "a b c d e"); do echo "Letter: ${var}"; done

for var in $(seq 0 20); do
	if (exit $((var % 3))); then
		echo "${var} is divisible by 3"
	fi
done

for i in $(seq 1 5); do
	echo "Start outer loop: ${i}"
	for j in $(seq 1 ${i}); do
		echo "  Inner loop: ${j}"
	done
done

for i in $(seq 1 3); do
	for j in $(seq 3 -1 1); do
		echo "i=${i}, j=${j}"
	done
done

for var in $(seq 10 1); do
	(exit $((var > 5))) || echo "${var} is less than -5"
done

for var in $(seq 1 2 20); do echo "${var}"; done
for var in $(seq 10 -2 0); do echo "${var}"; done

for var in $(seq 1 10); do
	echo "${var} $(seq ${var})"
done

for var in $(seq 1 10); do echo "$(echo ${var} | tr '0-9' 'a-j')"; done
for var in $(seq 1 10); do echo "${var} $(seq $((var * 2)))"; done

# # for var in $(seq 1 10); do
# # 	if [ ${var} -eq 5 ]; then
# # 		echo "Breaking at ${var}"
# # 		break
# # 	fi
# # done
# #
# # for var in $(seq 1 10); do
# # 	if [ ${var} -eq 5 ]; then
# # 		echo "Skipping ${var}"
# # 		continue
# # 	fi
# # 	echo "Value: ${var}"
# # done
# #
# # for var in $(seq 0 10); do
# # 	if [ ${var} -gt 5 ]; then
# # 		break
# # 	fi
# # 	echo "Value: ${var}"
# # done
# #
# # for var in $(seq 1 10); do
# # 	if [ ${var} -eq 5 ]; then
# # 		continue
# # 	fi
# # 	echo "Value: ${var}"
# # done
#
