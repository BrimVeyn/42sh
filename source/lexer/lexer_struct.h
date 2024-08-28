/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_struct.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 15:33:36 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/27 16:35:41 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_STRUCT_H
#define LEXER_STRUCT_H

//All tag enums
#include <stdint.h>
#pragma once
#include "lexer_enum.h"
#include "lexer.h"

#define DELIMITERS_DEFAULT "|$;\0\n"
#define DELIMITERS_DQUOTE "$\0"

typedef struct Token {
	type_of_token tag;
	type_of_error e;
	uint16_t parent;
	union {
		struct {
			type_of_separator s_type;
		}; //separator

		struct {
			type_of_redirection r_type;
			struct Token *r_postfix;
		};  //redirection

		struct {
			char			*w_infix;
			struct Token	*w_postfix;
		}; //word

		struct {
			type_of_grouping		g_type;
			struct TokenList		*g_list;
			struct Token			*g_postfix;
		}; // idk bru
	};
} Token;




typedef struct {
	type_of_error	error;
	char			*error_message;
} Error;

#endif // !LEXER_STRUCT_H
