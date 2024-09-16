/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 15:05:05 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/16 13:15:40 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"
#include <dirent.h>
#include <stdio.h>

bool is_operator(type_of_separator s) {
	return s == S_BG || s == S_EOF || s == S_OR || s == S_AND || s == S_SEMI_COLUMN || s == S_NEWLINE || s == S_PIPE;
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
	return gc_add(ft_strdup(filename), GC_GENERAL);
}

bool heredoc_detector(TokenList *data) {
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

RedirectionList *parser_get_redirection(TokenList *tl) {
	RedirectionList *redir_list = redirection_list_init();
	for (uint16_t it = 0; it < tl->size; it++) {
		const Token *el = tl->t[it];
		if (el->tag == T_REDIRECTION ||
			(el->tag == T_WORD && el->w_postfix->tag == T_REDIRECTION))
		{
			const Token *next = (el->tag == T_WORD && el->w_postfix->tag == T_REDIRECTION) ? el->w_postfix : el;
			Redirection *current_redir = (Redirection *) gc_add(ft_calloc(1, sizeof(Redirection)), GC_GENERAL);

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
	SimpleCommand *curr_command = (SimpleCommand *) gc_add(ft_calloc(1, sizeof(SimpleCommand)), GC_SUBSHELL);

	size_t count = 0;
	for (uint16_t it = 0; it < tl->size; it++) {
		const Token *el = tl->t[it];
		count += (el->tag == T_WORD && el->w_postfix->tag != T_REDIRECTION);
	}

	curr_command->args = (char **) gc_add(ft_calloc(count + 1, sizeof(char *)), GC_SUBSHELL);

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


//echo 0 ; echo 1 | (echo 2 && echo 3) | echo 4 && echo 5
//echo 1 | (echo 2 && echo 3) | echo 4 && echo 5
//echo 1 | (echo 2 && echo 3) | echo 4
//echo -2 | echo -1 | echo 0; echo 1 | echo 2

void skip_subshell_parser(TokenList *list, int *j) {
	(*j)++;
	while (!is_end_sub(list, j)) {
		if (is_subshell(list, j)) {
			skip_subshell_parser(list, j);
		}
		(*j)++;
	}
}

type_of_separator next_separator(TokenList *list, int *i) {
	int j = *i;
	while (j < list->size) {
		if (is_subshell(list, &j)) {
			skip_subshell_parser(list, &j);
		}
		if (is_semi(list, &j) || is_eof(list, &j) || is_pipe(list, &j)) {
			return list->t[j]->s_type;
		}
		(j)++;
	}
	return S_EOF;
}

void skip_subshell(TokenList *newlist, TokenList *list, int *i) {
	token_list_add(newlist, list->t[*i]);
	(*i)++;
	while (*i < list->size && !is_end_sub(list, i)) {
		if (is_subshell(list, i)) {
			skip_subshell(newlist, list, i);
		}
		if (*i < list->size) {
			token_list_add(newlist, list->t[*i]);
			(*i)++;
		}
	}
}

TokenList *extract_subshell_rec(TokenList *list, int *i) {
	TokenList *newlist = token_list_init();
	(*i)++;
	while (*i < list->size && !is_end_sub(list, i)) {
		if (*i < list->size && is_subshell(list, i)) {
			skip_subshell(newlist, list, i);
		}
		if (*i < list->size) {
			token_list_add(newlist, list->t[*i]);
			(*i)++;
		}
	}
	(*i) += 1;
	return newlist;
}

Node *extract_subshell(TokenList *list, int *i) {
	TokenList *newlist = extract_subshell_rec(list, i);
	(*i) += 1;
	if (g_debug > 2) {
		printf("============================================\n");
		tokenToStringAll(newlist);
		printf("============================================\n");
	}
	return ast_build(newlist);
}

TokenList *extract_tokens(TokenList *list, int *i) {
	TokenList *newlist = token_list_init();
	while (*i < list->size && !is_pipe(list, i) && !is_eof(list, i) && !is_semi(list, i)) {
		token_list_add(newlist, list->t[*i]);
		(*i)++;
	}
	(*i)++;
	return newlist;
}

ExecuterList *build_executer_list(TokenList *list) {
	// if (true){
	// 	printf(C_RED"----------Before EXECUTER-------------"C_RESET"\n");
	// 	tokenToStringAll(list); //Debug
	// }
	ExecuterList *self = executer_list_init();
	int i = 0;
	while (i < list->size) {
		if (next_separator(list, &i) == S_PIPE) {
			Executer *executer = NULL;
			while (next_separator(list, &i) == S_PIPE) {
				// dprintf(2, "je sui la !\n");
				if (is_subshell(list, &i)) {
					Executer *new = executer_init(extract_subshell(list, &i), NULL);
					executer_push_back(&executer, new);
				} else {
					Executer *new = executer_init(NULL, extract_tokens(list, &i));
					executer_push_back(&executer, new);
				}
			}
			if (i < list->size) {
				if (is_subshell(list, &i)) {
					Executer *new = executer_init(extract_subshell(list, &i), NULL);
					executer_push_back(&executer, new);
				} else {
					Executer *new = executer_init(NULL, extract_tokens(list, &i));
					executer_push_back(&executer, new);
				}
			}
			executer_list_push(self, executer);
		}
		if (i < list->size && (next_separator(list, &i) == S_EOF || next_separator(list, &i) == S_SEMI_COLUMN)) {
			Executer *executer = NULL;
			if (is_subshell(list, &i)) {
				Executer *new = executer_init(extract_subshell(list, &i), NULL);
				executer_push_back(&executer, new);
			} else {
				Executer *new = executer_init(NULL, extract_tokens(list, &i));
				executer_push_back(&executer, new);
			}
			executer_list_push(self, executer);
		}
	}
	return self;
}

SimpleCommand *parser_parse_current(TokenList *tl, StringList *env, int *saved_fds) {
	
	// parser_brace_expansion();
	// parser_tilde_expansion();

	//AU BOULOT
	if (!parser_parameter_expansion(tl, env)){
		return NULL;
	}
	if (!parser_command_substitution(tl, env, saved_fds)) {
		return NULL;
	}
	// parser_arithmetic_expansion();
	// parser_word_splitting();
	// if (!parser_filename_expansion(tl)){
	// 	return NULL;
	// }
	// tokenToStringAll(tl);
	// parser_quote_remioval();
	RedirectionList *redirs = parser_get_redirection(tl);
	SimpleCommand *command = parser_get_command(tl);
	command->redir_list = redirs;
	
	return command;
}
