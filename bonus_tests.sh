#!/bin/bash

num=11
if [ $num -gt 5 ]; then



	echo "The number is greater than 5"
	echo "salut !"
else
	echo "The nunber is lower than 5"
fi

num=10;
if [ "$num" -gt 5 ]; then echo salut; fi;

if [[ "$num" -gt 1 ]]; then
	echo salut; 
elif [ "$num" -gt 12 ]; then
	echo second;
fi;

if if [[ $var -lt 10 ]]; then
	echo test nested ifs;
	((var++));
	true;
fi
then if [[ $var -lt 10 ]]; then

	if true; then
		echo thirs nested;
	fi;
	echo test second if;
	fi;
	echo test third if;
fi;

# if
# any
# then 
# any
# elif | else | fi
#
# case elif --> any
# then
# any
# elif | else | fi
#
# case else --> any
# fi
#
# case fi


# if false; then
# 	echo first;
# else if true; then
# 		echo second;
# 	fi;
# fi;
#
# while echo salut; do
#     echo "var contains only digits"
# 	num=w;
# done;
