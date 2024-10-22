#!/bin/bash

# num=11
# if [ $num -gt 5 ]; then
#
#
#
# 	echo "The number is greater than 5"
# 	echo "salut !"
# else
# 	echo "The nunber is lower than 5"
# fi
#
# num=10;
# if [ "$num" -gt 5 ]; then echo salut; fi;
#
# if [[ "$num" -gt 1 ]]; then
# 	echo salut; 
# elif [ "$num" -gt 12 ]; then
# 	echo second;
# fi;
#
# if if [[ $var -lt 10 ]]; then
# 	echo test nested ifs;
# 	((var++));
# 	true;
# fi
# then if [[ $var -lt 10 ]]; then
#
# 	if true; then
# 		echo thirs nested;
# 	fi;
# 	echo test second if;
# 	fi;
# 	echo test third if;
# fi;

if true; then
	echo outer_true;
	if true; then
		echo inner_true;
	fi
elif true; then
	echo outer_second_true;
else
	echo outer_false;
fi

if AST1; then
	AST2;
elif AST3; then
	AST4;
else
	AST5;
fi

AST1 = SimpleCommand("true");
AST2 = SimpleCommand("echo outer_true;") --> IFNODE;
AST3 = SimpleCommand("true")
AST4 = SimpleCommand("echo outer_second_true");
AST5 = SimpleCommand("outer_false");

if isTruthy(AST1.execute) then AST2.execute
else if isTruthy(AST3) then AST4.execute
else AST5.execute
