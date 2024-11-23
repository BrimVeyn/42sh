/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 16:39:47 by nbardavi          #+#    #+#             */
/*   Updated: 2024/11/23 22:29:51 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_H
# define DEBUG_H

#include "parser.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

extern int g_debug;
//---------------AST----------------------//
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
