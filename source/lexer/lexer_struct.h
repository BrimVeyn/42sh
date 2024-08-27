/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_struct.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 13:46:46 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/27 14:43:28 by nbardavi         ###   ########.fr       */
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

typedef enum {
	R_FD,
	R_FILENAME,
} type_of_suffix;

typedef struct {
	int prefix_fd;
	type_of_redirection r_type;
	type_of_suffix su_type;
	union {
		int fd;
		char *filename;
	};
} Redirection;

typedef struct TokenList {
	Token		**t;
	uint16_t	size;
	uint16_t	capacity;
} TokenList;

typedef struct RedirectionList {
	Redirection		**r;
	uint16_t	size;
	uint16_t	capacity;
} RedirectionList;

typedef struct {
	type_of_error	error;
	char			*error_message;
} Error;

#endif // !LEXER_STRUCT_H
