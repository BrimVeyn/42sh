/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_enum.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:04:50 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/21 15:09:21 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_ENUM_H
#define LEXER_ENUM_H


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
	T_END_OF_FILE,
	//---------------
	T_NONE, //w/0 prefix | suffix
} type_of_token;

typedef enum {
	ERROR_UNEXPCTED_TOKEN,
} type_of_error;

typedef enum {
	EX_WORD,
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
	CG_CONTROL_SUBSTITUTION,
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
char *get_tagName_token(type_of_token type);
char *get_tagName_command_grouping(type_of_command_grouping type);

#define tagName(param) _Generic((param), \
	type_of_redirection: get_tagName_redirection,	\
	type_of_expression: get_tagName_expression,		\
	type_of_token: get_tagName_token,				\
	type_of_command_grouping: get_tagName_command_grouping,				\
    default : printf("Format not handled ! (tagName)\n") \
)(param)

#endif // !LEXER_ENUM_H
