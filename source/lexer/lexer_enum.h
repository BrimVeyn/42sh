/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_enum.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:04:50 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/04 17:10:47 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_ENUM_H
#define LEXER_ENUM_H

typedef enum {
	T_COMMAND,
	//---------------
	T_REDIRECTION, //ok
	//---------------
	T_GROUPING,
	//---------------
	T_WORD, // generic word ?
	//---------------
	T_SEPARATOR,
	//---------------
	T_NONE, // Generic none token when no prefix/suffix
} type_of_token;

typedef enum {
	ERROR_NONE, //error free 
	ERROR_UNEXPECTED_TOKEN, //<> ||| <<<< <<<>
	ERROR_UNCLOSED_SQ, //'....\0
	ERROR_UNCLOSED_DQ, //'....\0
	ERROR_UNCLOSED_CG, //".....\0
	ERROR_BAD_SUBSTITUTION, // cf invalid identifier
	ERROR_ESCAPED_SQ, //'..\'..'
} type_of_error;

typedef enum {
	S_AND,// &&
	S_OR, // ||
	S_PIPE, // |
	S_SEMI_COLUMN, // ;
	S_BG, // &
	S_NEWLINE, // \n
	S_EOF, // \0
	S_CMD_SUB, // $(
	S_SUB_OPEN, // (
	S_SUB_CLOSE, // )
	S_DQ, //"
	S_DEFAULT,
} type_of_separator;

typedef enum {
	R_OUTPUT, //>[ex|n]
	R_APPEND, //>>[ex|n]
	R_INPUT, //[?n]<[ex|n]
	R_HERE_DOC, //[?n]<<[delimiter]
	R_DUP_IN, //[?n]<&[ex|n]
	R_DUP_OUT, //[?n]>&[ex|n]
	R_DUP_BOTH, //&>[ex|n]
	R_DUP_BOTH_APPEND, //&>>[ex]
} type_of_redirection;

typedef enum {
	C_BUILTIN, //echo, etc...
	C_SHELL, //grep, etc...
} type_of_command;

#endif // !LEXER_ENUM_H
