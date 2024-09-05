/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 16:39:47 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/05 09:39:43 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_H
# define DEBUG_H

#include "../../include/42sh.h"

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

//---------------Lexer debug---------------//
void lexer_debug(Lexer_p lexer);
void tokenToString(Token *t, size_t offset);
void tokenToStringAll(TokenList *t);

//---------------AST----------------------//
void printTree(Node *self);
void tokenListToStringAll(TokenListVector *cont);


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
