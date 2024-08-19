/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_enum.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:04:50 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/16 18:17:47 by bvan-pae         ###   ########.fr       */
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
	//---------------
	T_SEPARATOR,
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
	S_AMPERSAND, // &
} type_of_separator;

typedef enum {
	R_INPUT, //<
	R_OUTPUT, //>
	R_APPEND, //>>
	R_HERE_DOC, //<<
	R_AMPERSAND, //<& //>&
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
