/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 16:22:21 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/02 17:23:54 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

extern int debug;

bool is_operator(type_of_separator s) {
	return s == S_BG || s == S_EOF || s == S_OR || s == S_AND || s == S_SEMI_COLUMN || s == S_NEWLINE || s == S_PIPE;
}

TokenList *lexer_lex_till_operator(Parser *p) {
	const TokenList *list = p->data;
	TokenList *self = token_list_init();
	while (p->it < list->size) {
		Token *tmp = list->t[p->it++];
		token_list_add(self, tmp);
		if (tmp->tag == T_SEPARATOR && is_operator(tmp->s_type))
			break;
	}
	return self;
}

TokenList *lexer_lex_all(Parser *p) {
	const Lexer_p l = p->lexer;
	TokenList *self = token_list_init();
	while (true) {
		Token *tmp = lexer_get_next_token(l, false, DEFAULT);
		token_list_add(self, tmp);
		if (tmp->tag == T_SEPARATOR && tmp->s_type == S_EOF) break;
	}
	return self;
}

bool syntax_error_detector(Parser *p) {
	const TokenList *data = p->data;
	for (uint16_t it = 0; it < data->size; it++) {
		const Token *el = data->t[it];
		if (el->tag == T_SEPARATOR && it == 0) {
			dprintf(STDERR_FILENO, UNEXPECTED_TOKEN_STR"`%s\'\n", tagName(el->s_type));
			return false;
		}
		if (el->tag == T_SEPARATOR && it == data->size - 2 &&
			(el->s_type == S_AND || el->s_type == S_OR || el->s_type == S_PIPE)) {
			dprintf(STDERR_FILENO, UNEXPECTED_TOKEN_STR"`%s\'\n", tagName(el->s_type));
			return false;
		}
		if (el->e != ERROR_NONE) {
			const Token *redir = (el->tag == T_REDIRECTION) ? el : el->w_postfix;
			tokenToString((Token *)el, 0);
			dprintf(STDERR_FILENO, UNEXPECTED_TOKEN_STR"`%s\'\n", tagName(redir->r_type));
			return false;
		}
	}
	return true;
}

char *here_doc(char *eof){
	char *input = NULL;
	static int heredoc_number = 0;
	char filename[50]; 
	sprintf(filename, "/tmp/here_doc_%d", heredoc_number++);
	int file_fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0664);
	if (file_fd == -1) {
		return NULL;
	}
	while((input = readline("> ")) && ft_strcmp(eof, input)){
		dprintf(file_fd, "%s\n", input);
	}
	close(file_fd);
	return gc_add(ft_strdup(filename));
}

bool heredoc_detector(Parser *p) {
	const TokenList *data = p->data;
	for (uint16_t it = 0; it < data->size; it++) {
		const Token *curr = data->t[it];
		Token *const el = (curr->tag == T_WORD && curr->w_postfix->tag == T_REDIRECTION) ? curr->w_postfix : (Token *) curr;
		if (el->tag == T_REDIRECTION && el->r_type == R_HERE_DOC) {
			Token *filename = el->r_postfix;
			el->r_type = R_INPUT;
			filename->w_infix = here_doc(filename->w_infix);
			if (!filename->w_infix) return false;
		}
	}
	return true;
}

Parser *parser_init(char *input) {
	Parser *self = (Parser *) gc_add(ft_calloc(1, sizeof(Parser)));

	self->lexer = lexer_init(input);
	self->it = 0;
	self->data = lexer_lex_all(self);
	self->curr_command = lexer_lex_till_operator(self);
	self->peak_command = lexer_lex_till_operator(self);
	return self;
}

void parser_get_next_command(Parser *self) {
	self->curr_command = self->peak_command;
	self->peak_command = lexer_lex_till_operator(self);
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
		if (el->tag == T_REDIRECTION ||
			(el->tag == T_WORD && el->w_postfix->tag == T_REDIRECTION))
		{
			const Token *next = (el->tag == T_WORD && el->w_postfix->tag == T_REDIRECTION) ? el->w_postfix : el;
			Redirection *current_redir = (Redirection *) gc_add(ft_calloc(1, sizeof(Redirection)));

			current_redir->prefix_fd = (next != el) ? ft_atoi(el->w_infix) : -1;
			current_redir->r_type = next->r_type;
			if (is_number(next->r_postfix->w_infix) &&
				(next->r_type == R_DUP_OUT ||
				next->r_type == R_DUP_IN)) {
				current_redir->su_type = R_FD;
				current_redir->fd = ft_atoi(next->r_postfix->w_infix);
			} else {
				current_redir->su_type = R_FILENAME;
				current_redir->filename = next->r_postfix->w_infix;
			}
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
		count += (el->tag == T_WORD && el->w_postfix->tag != T_REDIRECTION);
	}

	curr_command->args = (char **) gc_add(ft_calloc(count + 1, sizeof(char *)));

	size_t i = 0;
	for (uint16_t it = 0; it < tl->size; it++) {
		const Token *el = tl->t[it];
		if (el->tag == T_WORD && el->w_postfix->tag != T_REDIRECTION) {
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

void parser_parameter_expansion(TokenList *tl){
	(void)tl;
}

SimpleCommand *parser_parse_current(TokenList *tl) {
	// parser_brace_expansion();
	// parser_tilde_expansion();
	parser_parameter_expansion(tl);
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
		printRedirList(curr->redir_list);
		printf(C_GOLD"------ "C_LIGHT_YELLOW"Command"C_RESET C_GOLD"-------\n"C_RESET);
		printf("bin: %s\n", command->bin);
		printCharChar(curr->args);
		printf(C_GOLD"---------...---------"C_RESET"\n");
		curr = curr->next;
	}
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
		parser_get_next_command(self);
		*next_separator = cut_separator(self->curr_command);
		SimpleCommand *next_command = parser_parse_current(self->curr_command);

		curr->next = next_command;
		curr = curr->next;
	}
}

void parser_parse_all(Parser *self, char **env) {
	static int exitno = 0;
	(void)exitno;
	while (true) {
		type_of_separator next_seperator = cut_separator(self->curr_command);

		const Token *first_element = self->curr_command->t[0];
		if (first_element->tag == T_SEPARATOR && first_element->s_type == S_EOF) break;

		SimpleCommand *command = parser_parse_current(self->curr_command);
		if (next_seperator == S_PIPE) {
			fill_pipeline(self, command, &next_seperator);
		}

		if (debug){
			printCommand(command); //Debug
		}
		exitno = exec_simple_command(command, env);

		if (next_seperator == S_EOF) break;
		parser_get_next_command(self);
	}
}
