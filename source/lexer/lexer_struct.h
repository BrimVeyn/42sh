/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_struct.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 13:55:47 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/27 11:29:46 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_STRUCT_H
#define LEXER_STRUCT_H

//All tag enums
#pragma once
#include "lexer_enum.h"
#include "lexer.h"

#define DELIMITERS_DEFAULT "|$;\0\n"
#define DELIMITERS_DQUOTE "$\0"

typedef struct Token {
	type_of_token tag;
	type_of_error e;
	char *delimiters;
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
			Lexer_p					g_lexer;
			struct TokenList		*g_list;
			struct Token			*g_postfix;
		}; // idk bru
	};
} Token;

typedef struct {
	char *bin;
	char **args;
} SimpleCommand;

typedef struct TokenList {
	Token		**t;
	uint16_t	size;
	uint16_t	capacity;
} TokenList;

typedef struct {
	type_of_error	error;
	char			*error_message;
} Error;

#endif // !LEXER_STRUCT_H
