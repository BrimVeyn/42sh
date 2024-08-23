/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_enum.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:04:50 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/23 13:25:02 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_ENUM_H
#define LEXER_ENUM_H

#include <stdint.h>
typedef enum {
	T_COMMAND,
	//---------------
	T_REDIRECTION, //ok
	T_EXPRESSION, //last
	//---------------
	T_COMMAND_GROUPING,
	//---------------
	T_SEPARATOR,
	//---------------
	T_END_OF_FILE,
	//---------------
	T_NONE, // Generic none token when no prefix/suffix
} type_of_token;

typedef enum {
	ERROR_NONE, //error free 
	ERROR_UNEXPCTED_TOKEN, //<> ||| <<<< <<<>
	ERROR_UNCLOSED_SQ, //'....\0
	ERROR_UNCLOSED_DQ, //'....\0
	ERROR_UNCLOSED_CG, //".....\0
	ERROR_BAD_SUBSTITUTION, // cf invalid identifier | brace expansion ? maybe
	ERROR_ESCAPED_SQ, //'..\'..'
} type_of_error;

typedef enum {
	EX_WORD, // [word]
	EX_VARIABLE_ASSIGNMENT, //[expr]=[expr]
	EX_VARIABLE_EXPANSION, //${[identifier]}
	EX_BRACES_EXPANSION, //{[range|enum]}
	EX_PATTERN_MATCHING, //*[]?!
	EX_INHIBITOR_SQUOTE, //"[any]"
	EX_INHIBITOR_DQUOTE, //"[any]"
	EX_ERROR, //Error ?
	// EX_ARITHMETIC_EXPRESSION, //eventually
} type_of_expression;

typedef enum {
	S_AND,// &&
	S_OR, // ||
	S_PIPE, // |
	S_SEMI, // ;
	S_BG, // &
} type_of_separator;

typedef enum {
	CG_SUBSHELL, //([any])
	CG_CONTROL_GROUP, //$([any])
	CG_CONTROL_SUBSTITUTION, //{ [any]; }
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

const char *get_tagName_redirection(type_of_redirection type);
const char *get_tagName_expression(type_of_expression type);
const char *get_tagName_token(type_of_token type);
const char *get_tagName_command_grouping(type_of_command_grouping type);
const char *get_tagName_error(type_of_error type);
const char *get_tagName_separator(type_of_separator type);
const char *error_handler(uintptr_t type);

#define tagName(param) _Generic((param),					\
	type_of_redirection: get_tagName_redirection,			\
	type_of_expression: get_tagName_expression,				\
	type_of_token: get_tagName_token,						\
	type_of_command_grouping: get_tagName_command_grouping,	\
	type_of_error: get_tagName_error,						\
	type_of_separator: get_tagName_separator,				\
    default: error_handler									\
)(param)

#endif // !LEXER_ENUM_H
