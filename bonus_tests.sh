#!/bin/bash

pipeline         :      pipe_sequence
                 | Bang pipe_sequence
                 ;

separator_op     : '&'
                 | ';'

command          : simple_command
                 | compound_command
                 | compound_command redirect_list
                 | function_definition

compound_command : brace_group
                 | subshell
                 | for_clause
                 | case_clause
                 | if_clause
                 | while_clause
                 | until_clause

pipe_sequence    :                             command
                 | pipe_sequence '|' linebreak command

and_or           :                         pipeline
                 | and_or AND_IF linebreak pipeline
                 | and_or OR_IF  linebreak pipeline

separator        : separator_op linebreak
                 | newline_list

term             : term separator and_or
                 |                and_or

newline_list     :              NEWLINE
                 | newline_list NEWLINE
                 ;
linebreak        : newline_list
                 | /* empty */

compound_list    : linebreak term
                 | linebreak term separator

if_clause        : If compound_list Then compound_list else_part Fi
                 | If compound_list Then compound_list           Fi

if echo salut; then echo hey; fi
if echo salut; then echo hey; fi
if if echo salut; then true; fi; then echo hey; fi
if if echo salut; then true; fi while false; do true; done then echo hey; fi
if if echo salut; then true; fi; echo salut; then echo hey; fi
if (echo salut) then echo hey; fi;

construction = (IF | WHILE | SUBSHELL | CMD_GRP)

Si construction et dans une autre alors pas besoin


# struct Process {
# 	struct Process *next;
# 	struct Expr *data;
# }
#
# struct Node {
# 	Expr data;
# 	Node *lhs;
# 	Node *rhs;
# }
#
# struct Expr {
# 	tagged union {
# 		TokenList *;
# 		IfStruct *;
# 		Separator ;
# 		SubShell ;
# 	}
# }
#
# struct ExprArray {
# 	Expr *data;
# 	size_t cap;
# 	size_t size;
# }
#
# Expr(IfStruct({
# 	.condition = Epxr(TokenList) Expr(;)
# 	.body = Expr(TokenList) Expr(|) Expr(IfStruct({
# 	.condition = Expr(SubShell{
# 		Expr(IfStruct({
# 			.condition = Expr(TokenLisT) Expr(;)
# 			.body = Expr(TokenLisT) Expr(;)
# 			.else = Nil
# 		}))
# 	}) Expr(;)
# 	.else = Nil
# })) Expr(|) Expr(TokenLisT) Expr(|) Expr(TokenLisT) Expr(;) Expr(TokenList)
#
# Expr(echo) Expr(salut) Expr(etc) Expr('|') Expr(rev)
