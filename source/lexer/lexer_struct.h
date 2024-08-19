/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_struct.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 13:55:47 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/16 18:18:27 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_STRUCT_H
#define LEXER_STRUCT_H

//All tag enums
#include "lexer_enum.h"

typedef struct {
	type_of_token tag;
	union {
		struct {
			type_of_redirection r_type;
			union {
				char *filename;
				int  *fd;
			}; //either a filename or a fd number
		};  //redirection

		struct {
			type_of_command c_type;
			char *binary;
			char *args;
		};//shell command

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

#endif // !LEXER_STRUCT_H
