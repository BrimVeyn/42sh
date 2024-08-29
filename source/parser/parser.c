/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 13:37:47 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/29 09:52:40 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "../exec/exec.h"
#include "../debug/debug.h"

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

	curr_command->args = (char **) gc_add(ft_calloc(count + 1, sizeof(char *)));

	size_t i = 0;
	for (uint16_t it = 0; it < tl->size; it++) {
		const Token *el = tl->t[it];
		if (el->tag == T_WORD) {
			if (i == 0) {
				curr_command->bin = el->w_infix;
			}
			curr_command->args[i++] = el->w_infix;
		}
	}
	curr_command->args[i] = NULL;
	curr_command->next = NULL;
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
	printf(C_BRONZE"------ "C_LIGHT_BROWN"Redir list"C_BRONZE"----\n"C_RESET);
	for (uint16_t it = 0; it < rl->size; it++) {
		const Redirection *el = rl->r[it];
		printf("prefix_fd ["C_BRONZE"%d"C_RESET"]:\t"C_LIGHT_BROWN"%d\n"C_RESET, it, el->prefix_fd);
		printf("r_type    ["C_BRONZE"%d"C_RESET"]:\t"C_LIGHT_BROWN"%s\n"C_RESET, it, tagName(el->r_type));
		printf("su_type   ["C_BRONZE"%d"C_RESET"]:\t"C_LIGHT_BROWN"%s\n"C_RESET, it, tagName(el->su_type));
		if (el->su_type == R_FD) {
			printf("suffix_fd ["C_BRONZE"%d"C_RESET"]:"C_LIGHT_BROWN"\t%d\n"C_RESET, it, el->fd);
		} else if (el->su_type == R_FILENAME) {
			printf("filename  ["C_BRONZE"%d"C_RESET"]:\t"C_LIGHT_BROWN"%s\n"C_RESET, it, el->filename);
		}
		if (it + 1 < rl->size) {
			printf(C_BRONZE"------------------\n"C_RESET);
		}
	}
	printf(C_BRONZE"---------...---------\n"C_RESET);

}

void printCharChar(char **tab) {
	for (uint16_t it = 0; tab[it]; it++) {
		printf("arg[%d]: %s\n", it, tab[it]);
	}
}

void printCommand(SimpleCommand *command) {
	SimpleCommand *curr = command;
	while (curr != NULL) {
		printf("HOOOO\n");
		printRedirList(curr->redir_list);
		printf(C_GOLD"------ "C_LIGHT_YELLOW"Command"C_GOLD"-------\n"C_RESET);
		printf("bin: %s\n", command->bin);
		printCharChar(curr->args);
		printf(C_GOLD"---------...---------\n"C_RESET);
		curr = curr->next;
	}
}

bool has_reached_eof(const TokenList *tl) {
	const Token *last_el = tl->t[tl->size - 1];
	return last_el->tag == T_SEPARATOR && last_el->s_type == S_EOF;
}

type_of_separator cut_separator(TokenList *tl) {
	const Token *last_el = tl->t[tl->size - 1];
	if (last_el->tag == T_SEPARATOR) {
		tl->size -= 1;
		return last_el->s_type;
	}
	return S_DEFAULT;
}

void fill_pipeline(Parser *self, SimpleCommand *command, type_of_separator *next_separator) {
	SimpleCommand *curr = command;
	while (*next_separator == S_PIPE) {
		// Fetch the next command
		parser_get_next_command(self);
		
		// Update the separator for the next loop iteration
		*next_separator = cut_separator(self->curr_command);
		
		// Parse the current command
		SimpleCommand *next_command = parser_parse_current(self->curr_command);
		
		// Link the current command to the next
		curr->next = next_command;
		
		// Move to the next command in the pipeline
		curr = curr->next;
	}
}

void parser_parse_all(Parser *self) {
	//define null operator
	while (true) {
		type_of_separator next_seperator = cut_separator(self->curr_command);
		const Token *first_element = self->curr_command->t[self->curr_command->size - 1];
		if (first_element->tag == T_SEPARATOR && first_element->s_type == S_EOF) break;
		SimpleCommand *command = parser_parse_current(self->curr_command);
		if (next_seperator == S_PIPE) {
			fill_pipeline(self, command, &next_seperator);
		}
		printf("seperator = %s\n", tagName(next_seperator));
		printCommand(command); //Debug
		exec_simple_command(command);
		printf("JE SUIS LA\n");
		if (next_seperator == S_EOF) break;
		parser_get_next_command(self);
	}
}

/*
 * TO DO 
 *
 * Color debug
 * check si le fd redirect est un nombre
 *
 * */
