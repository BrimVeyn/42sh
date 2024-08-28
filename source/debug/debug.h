/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 16:39:47 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/27 16:44:04 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_H
# define DEBUG_H

#pragma once
#include "../parser/parser.h"
#include "../lexer/lexer.h"

const char *get_tagName_token(type_of_token type);
const char *get_tagName_redirection(type_of_redirection type);
const char *get_tagName_grouping(type_of_grouping type);
const char *get_tagName_error(type_of_error type);
const char *get_tagName_separator(type_of_separator type);
const char *get_tagName_suffix(type_of_suffix type);
const char *error_handler(uintptr_t type);


#define tagName(param) _Generic((param),					\
	type_of_token: get_tagName_token,						\
	type_of_redirection: get_tagName_redirection,			\
	type_of_grouping: get_tagName_grouping,					\
	type_of_error: get_tagName_error,						\
	type_of_separator: get_tagName_separator,				\
	type_of_suffix: get_tagName_suffix,						\
    default: error_handler									\
)(param)

#endif
