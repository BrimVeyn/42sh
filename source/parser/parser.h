/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 13:45:31 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/27 15:24:02 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../lexer/lexer.h"

typedef struct {
	Lexer_p lexer;
	TokenList *curr_command;
	TokenList *peak_command;
} Parser;

Parser *parser_init(char *input);
void parser_get_next_command(Parser *self);
void parse_current(Parser *self);
void parser_print_state(Parser *self);
void parser_parse_all(Parser *self);
