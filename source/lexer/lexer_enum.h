/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_enum.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:04:50 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/20 18:16:52 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_ENUM_H
#define LEXER_ENUM_H

#include <stdio.h>
#define C_RESET       "\033[0m"
#define C_BLACK       "\033[0;30m"
#define C_RED         "\033[0;31m"
#define C_GREEN       "\033[0;32m"
#define C_YELLOW      "\033[0;33m"
#define C_BLUE        "\033[0;34m"
#define C_MAGENTA     "\033[0;35m"
#define C_CYAN        "\033[0;36m"
#define C_WHITE       "\033[0;37m"

typedef enum {
	//---------------
	T_REDIRECTION,
	T_EXPRESSION,
	T_COMMAND,
	//---------------
	T_PATTERN_MATCHING,
	T_CONTROL_GROUP,
	//---------------
	T_SEPARATOR,
	//---------------
	T_UNKNOWN_TYPE, //error ?
	//---------------
	T_NONE, //w/0 prefix | suffix
} type_of_token;

typedef enum {
	EX_WORD,
	EX_CONTROL_SUBSTITUTION,
	EX_VARIABLE_EXPANSION,
	EX_BRACES_EXPANSION,
	EX_PATTERN_MATCHING,
	EX_ERROR,
	// EX_ARITHMETIC_EXPRESSION,
} type_of_expression;

typedef enum {
	S_AND,// &&
	S_OR, // ||
	S_PIPE, // |
	S_SEMI, // ;
	S_BG, // &
} type_of_separator;

typedef enum {
	CG_SUBSHELL,
	CG_CONTROL_GROUP,
} type_of_command_grouping;

typedef enum {
	R_INPUT, //[?n]<[ex|n]
	R_OUTPUT, //[?n]>[ex|n]
	R_APPEND, //[?n]>>[ex|n]
	R_HERE_DOC, //[?n]<<[delimiter]
	R_DUP_IN, //[?n]<&[ex|n]
	R_DUP_OUT, //[?n]>&[ex|n]
	R_DUP_BOTH, //&>[ex|n]
	R_DUP_BOTH_APPEND, //&>>[ex]
	R_UNKNOWN, //error ?
} type_of_redirection;

typedef enum {
	C_BUILTIN, //echo, etc...
	C_SHELL, //grep, etc...
} type_of_command;

char *get_tagName_redirection(type_of_redirection type);
char *get_tagName_expression(type_of_expression type);

#define tagName(param) _Generic((param), \
	type_of_redirection: get_tagName_redirection,	\
	type_of_expression: get_tagName_expression,		\
    default : printf("Format not handled ! (tagName)\n") \
)(param)

#endif // !LEXER_ENUM_H
