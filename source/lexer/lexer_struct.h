/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_struct.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 13:55:47 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/15 16:57:56 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_STRUCT_H
#define LEXER_STRUCT_H

//All tag enums
#include "lexer_enum.h"

struct Range {
	int s;
	int e;
};

typedef union Token {
	enum type_of_token tag;

	struct {
		enum type_of_redirection type;
		char *filename;
	} redir; //redirection

	struct {
		enum type_of_command type;
	} cmd; //shell command

	struct {
	} exp; //shell expansion
	
	struct {
		enum type_of_separator type;
	} sep;

	struct {
		enum type_of_pattern_matching type;
	} pattern;

} Token;




#endif // !LEXER_STRUCT_H
