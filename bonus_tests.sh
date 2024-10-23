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
#


# struct Node {
# 	tagged union {
# 		TokensList *;
# 	}
# }


if true; then
	echo 1 | (if true; then echo 2; fi)
elif false; then
	echo 3;
fi | cat | rev && echo salut; test 1 + 1

struct Node {
	?? data;
	Node *lhs;
	Node *rhs;
}

struct Expr {
	tagged union {
		TokenList *;
		IfStruct *;
		Separator ;
		SubShell ;
	}
}

Expr(IfStruct({
	.condition = Epxr(TokenList) Expr(;)
	.body = Expr(TokenList) Expr(|) Expr(IfStruct({
	.condition = Expr(SubShell(IfStruct({
		.condition = Expr(TokenLisT) Expr(;)
		.body = Expr(TokenLisT) Expr(;)
		.else = Nil
	}))) Expr(;)
	.else = Nil
})) Expr(|) Expr(TokenLisT) Expr(|) Expr(TokenLisT) Expr(;) Expr(TokenList)
