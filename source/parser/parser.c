/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 13:50:04 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/27 16:08:21 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include <stdint.h>

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

RedirectionList *parser_get_redirection(TokenList *tl) {
	RedirectionList *redir_list = redirection_list_init();
	for (uint16_t it = 0; it < tl->size; it++) {
		const Token *el = tl->t[it];
		if (el->tag == T_REDIRECTION) {
			Redirection *current_redir = (Redirection *) gc_add(ft_calloc(1, sizeof(Redirection)));
			current_redir->prefix_fd = -1;
			current_redir->r_type = el->r_type;
			current_redir->filename = el->r_postfix->w_infix;
			current_redir->su_type = R_FILENAME;
			redirection_list_add(redir_list, current_redir);
		}
	}
	return redir_list;
}

SimpleCommand *parser_get_command(TokenList *tl) {
	SimpleCommand *curr_command = (SimpleCommand *) gc_add(ft_calloc(1, sizeof(SimpleCommand)));

	size_t count = 0;
	for (uint16_t it = 0; it < tl->size; it++) {
		const Token *el = tl->t[it];
		count += (el->tag == T_WORD);
	}

	curr_command->args = (char **) gc_add(ft_calloc(count, sizeof(char *)));

	size_t i = 0;
	for (uint16_t it = 0; it < tl->size; it++) {
		const Token *el = tl->t[it];
		if (el->tag == T_WORD) {
			if (i == 0) {
				curr_command->bin = el->w_infix;
				continue;
			}
			curr_command->args[i++] = el->w_infix;
		}
	}
	curr_command->args[i] = NULL;
	return curr_command;
}

SimpleCommand *parser_parse_current(TokenList *tl) {
	// parser_brace_expansion();
	// parser_tilde_expansion();
	// parser_parameter_expansion();
	// parser_command_substitution();
	// parser_arithmetic_expansion();
	// parser_word_splitting();
	// parser_filename_expansion();
	// parser_quote_removal();
	RedirectionList *redirs = parser_get_redirection(tl);
	SimpleCommand *command = parser_get_command(tl);
	command->redir_list = redirs;
	return command;
}

void printRedirList(RedirectionList *rl) {
	printf("------ Redir list ----\n");
	for (uint16_t it = 0; it < rl->size; it++) {
		const Redirection *el = rl->r[it];
		printf("el->prefix_fd: %d\n", el->prefix_fd);
		printf("el->r_type: %s\n", tagName(el->r_type));
		//add tagnam for su_type; LARBIN
		printf("el->filname: %s\n", el->filename);
	}
}

void printCharChar(char **tab) {
	for (uint16_t it = 0; tab[it]; it++) {
		printf("arg[%d]: %s\n", it, tab[it]);
	}
}

void printCommand(SimpleCommand *command) {
	printRedirList(command->redir_list);
	printf("bin: %s\n", command->bin);
	printCharChar(command->args);
}

void parser_parse_all(Parser *self) {
	while (self->curr_command->size != 0) {
		SimpleCommand *command = parser_parse_current(self->curr_command);
		printCommand(command);
		// exec_execute_command(self);
		parser_get_next_command(self);
	}
}


