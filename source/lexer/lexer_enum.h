/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_enum.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:04:50 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/19 15:46:31 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_ENUM_H
#define LEXER_ENUM_H

#include <stdio.h>

typedef enum {
	//---------------
	T_REDIRECTION,
	T_COMMAND,
	T_EXPANSION, 
	T_PATTERN_MATCHING,
	T_CONTROL_GROUP,
	T_CONTROL_SUBSTITUTION,
	//---------------
	T_SEPARATOR,
	//---------------
	T_WORD,
	//---------------
	T_UNKNOWN_TYPE //error ?
} type_of_token;

typedef enum {
	E_MATH,
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
	R_INPUT, //[?n]<[file|n]
	R_OUTPUT, //[?n]>[file|n]
	R_APPEND, //[?n]>>[file|n]
	R_HERE_DOC, //[?n]<<[delimiter]
	R_DUP_IN, //[?n]<&[n]
	R_DUP_OUT, //[?n]>&[n]
	R_DUP_BOTH, //&>[file] >&[file]
	R_DUP_BOTH_APPEND, //&>>[file]
	R_UNKNOWN, //error ?
} type_of_redirection;

typedef enum {
	C_BUILTIN, //echo, etc...
	C_SHELL, //grep, etc...
} type_of_command;

typedef enum {
	P_STAR,
	P_QUESTION_MARK,
	P_EXCLAMATION,
} type_of_pattern_matching;

char *get_tagName_redirection(type_of_redirection type);

#define tagName(param) _Generic((param), \
	type_of_redirection: get_tagName_redirection,	\
    default : printf("\n") \
)(param)

#endif // !LEXER_ENUM_H
