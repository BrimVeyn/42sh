/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_struct.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 13:55:47 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/19 17:10:34 by bvan-pae         ###   ########.fr       */
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
			type_of_redirection r_type;
			char		*filename;
			uint16_t	fd_prefix;
			uint16_t	fd_postfix;
		};  //redirection

		struct {
			char *w_word;
		}; //untyped word --> will be translated at parsing time

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
			type_of_expression e_type;
		}; //shell expansion
		
		struct {
			type_of_separator s_type;
		}; //separator

		struct {
			type_of_pattern_matching type;
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
		Token token;
	};
} Token_or_Error;

#endif // !LEXER_STRUCT_H
