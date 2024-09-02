/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_struct.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 15:33:36 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/02 08:55:30 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_STRUCT_H
#define LEXER_STRUCT_H

//All tag enums
#include <stdint.h>

#ifndef LEXER_H
	#include "lexer_enum.h"
	#include "lexer.h"

#endif // !LEXER_H

#define DELIMITERS_DEFAULT "|$;\0\n"
#define DELIMITERS_DQUOTE "$\0"

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
			char			*w_infix;
			struct Token	*w_postfix;
		}; //word
	};
} Token;




typedef struct {
	type_of_error	error;
	char			*error_message;
} Error;

#endif // !LEXER_STRUCT_H
