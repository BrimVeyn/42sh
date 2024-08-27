/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 13:50:04 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/27 15:28:49 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

Parser *parser_init(char *input) {
	Parser *self = (Parser *) gc_add(ft_calloc(1, sizeof(Parser)));

	self->lexer = lexer_init(input, DEFAULT);
	self->curr_command = lexer_lex_till_operator(self->lexer);
	self->peak_command = lexer_lex_till_operator(self->lexer);

	return self;
}

void parser_get_next_command(Parser *self) {
	self->curr_command = self->peak_command;
	self->peak_command = lexer_lex_till_operator(self->lexer);
}

void parser_print_state(Parser *self) {
	printf(C_RED"---- CURR_COMMAND------\n"C_RESET);
	tokenToStringAll(self->curr_command);
	printf(C_RED"----PEAK_COMMAND-------\n"C_RESET);
	tokenToStringAll(self->peak_command);
}

SimpleCommand *parser_parse_current(Parser *self) {
	// parser_brace_expansion();
	// parser_tilde_expansion();
	// parser_parameter_expansion();
	// parser_command_substitution();
	// parser_arithmetic_expansion();
	// parser_word_splitting();
	// parser_filename_expansion();
	// parser_quote_removal();
	RedirectionList *redirs = parser_get_redirection();
	SimpleCommand *command = parser_get_command();
}

void parser_parse_all(Parser *self) {
	while (self->curr_command != NULL) {
		parser_parse_current(self);
		// exec_execute_command(self);
		parser_get_next_command(self);
	}
}


