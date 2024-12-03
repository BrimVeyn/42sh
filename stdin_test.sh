var=10;

while true $((var--)); ! (exit $((var >= 0))); do
	echo ${var};
	sleep 0.01;
done

for positional; do
	echo ${positional}; #should output every positional arg passed to the script
done

for fruit in pomme poire banane $(echo ananas); do
	echo "Le fruit du jour est ${fruit}";
done;
