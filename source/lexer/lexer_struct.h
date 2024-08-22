/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_struct.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 13:55:47 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/22 13:59:00 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_STRUCT_H
#define LEXER_STRUCT_H

//All tag enums
#pragma once
#include "lexer_enum.h"
#include "lexer.h"

typedef struct Token {
	type_of_token tag;
	type_of_error e;
	union {

		struct {
			type_of_separator s_type;
		}; //separator

		struct {
			type_of_redirection r_type;
			struct Token *r_postfix;
		};  //redirection

		struct {
			type_of_command_grouping cg_type;
			Lexer_p cg_lexer;
			struct TokenList *cg_list;
			struct Token *cg_postfix;
		}; //Control substitution

		struct {
			type_of_command c_type;
			char *binary;
			char *args;
		};//shell command

		struct {
			type_of_expression ex_type;
			struct Token *ex_prefix;
			char *ex_infix;
			struct Token *ex_postfix;
		}; //shell expansion

		struct {
			char *pm_prefix;
			char *pm_infix;
			char *pm_postfix;
		}; //pattern_matching
	};
} Token;

typedef struct TokenList {
	Token **t;
	uint16_t size;
	uint16_t capacity;
} TokenList;

typedef enum {
	ERROR,
	TOKEN,
} token_or_error_tag;

typedef struct {
	type_of_error error;
	char *error_message;
} Error;

typedef struct {
	token_or_error_tag tag;
	union {
		Error e;
		Token *token; // or token
	};
} Token_or_Error;

#endif // !LEXER_STRUCT_H
