/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 16:39:47 by nbardavi          #+#    #+#             */
/*   Updated: 2024/11/06 16:09:20 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_H
# define DEBUG_H

#include "ast.h"
#include "parser.h"
#include "lexer.h"
#include "../source/parser/arithmetic.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

extern int g_debug;

//----------------Tagname---------------------//
const char *get_tag_name_token(type_of_token type);
const char *get_tag_name_redirection(type_of_redirection type);
const char *get_tag_name_error(type_of_error type);
const char *get_tag_name_separator(type_of_separator type);
const char *get_tag_name_suffix(type_of_suffix type);
const char *error_handler(uintptr_t type);

//----------------Tag Str---------------------//
const char *get_tag_str_redirection(type_of_redirection type);
const char *get_tag_str_separator(type_of_separator type);

//---------------Parser debug---------------//
void printCommand(SimpleCommand *command);
void aTokenToString(AToken *token);
void aTokenListToString(ATokenStack *tokens);

//---------------Lexer debug---------------//
void lexer_debug(Lexer_p lexer);
void tokenToString(Token *token, size_t offset);
void tokenListToString(TokenList *list);

//---------------AST----------------------//
void printTree(Node *self);
void expr_array_print(ExprArray *array);
void exp_kind_list_print(ExpKindList *list);

#define tagName(param) _Generic((param),					\
	type_of_token: get_tag_name_token,						\
	type_of_redirection: get_tag_name_redirection,			\
	type_of_error: get_tag_name_error,						\
	type_of_separator: get_tag_name_separator,				\
	type_of_suffix: get_tag_name_suffix,						\
    default: error_handler									\
)(param)

#define tagStr(param) _Generic((param),					\
	type_of_redirection: get_tag_str_redirection,			\
	type_of_separator: get_tag_str_separator,				\
    default: error_handler									\
)(param)

#endif
