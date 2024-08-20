/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_struct.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 13:55:47 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/20 16:59:23 by bvan-pae         ###   ########.fr       */
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
	union {

		struct {
			type_of_separator s_type;
		}; //separator

		struct {
			type_of_redirection r_type;
			int16_t	fd_prefix;
			int16_t	fd_postfix;
			struct Token *filename;
		};  //redirection

		struct {
			type_of_command c_type;
			char *binary;
			char *args;
		};//shell command

		struct {
			type_of_command_grouping cg_type;
			char *cg_input;
			Lexer_p cg_lexer;
			struct Token *cg_tok;
		}; //command grouping;

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

typedef enum {
	UNEXPECTED_TOKEN,
} syntax_error;

typedef enum {
	ERROR,
	TOKEN,
} token_or_error_tag;

typedef struct {
	token_or_error_tag tag;
	union {
		syntax_error error;
		Token *token;
	};
} Token_or_Error;

#endif // !LEXER_STRUCT_H
