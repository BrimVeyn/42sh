/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:27:46 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/16 16:29:09 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"
#include <stdio.h>

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

void add_redirection_from_token(RedirectionList **redir_list, const Token *el) {
	const Token *next = (el->tag == T_WORD && el->w_postfix->tag == T_REDIRECTION) ? el->w_postfix : el;
	Redirection *current_redir = (Redirection *) gc_add(ft_calloc(1, sizeof(Redirection)), GC_SUBSHELL);

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
	redirection_list_add(*redir_list, current_redir);
}

RedirectionList *parser_get_redirection(const TokenList *tl) {
	RedirectionList *redir_list = redirection_list_init();
	for (int it = 0; it < tl->size; it++) {
		const Token *el = tl->t[it];
		if (is_redirection(tl, &it))
			add_redirection_from_token(&redir_list, el);
	}
	return redir_list;
}

SimpleCommand *parser_get_command(const TokenList *tl) {
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
	return curr_command;
}

void parser_skip_subshell(TokenList *list, int *j) {
	(*j)++;
	while (!is_end_sub(list, j)) {
		if (is_subshell(list, j)) {
			parser_skip_subshell(list, j);
		}
		(*j)++;
	}
}

type_of_separator next_separator(TokenList *list, int *i) {
	int j = *i;
	while (j < list->size) {
		if (is_subshell(list, &j)) {
			parser_skip_subshell(list, &j);
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


RedirectionList *eat_redirections(TokenList *list, type_of_tree tag, int start) {
	if (tag == TREE_DEFAULT) return NULL;
	if (tag == TREE_SUBSHELL || tag == TREE_COMMAND_GROUP) {
		int i = start;

		if (i >= list->size) 
			return NULL;

		RedirectionList *redirs = redirection_list_init();

		while (i < list->size) {
			const Token *el = list->t[i];
			if (is_redirection(list, &i))
			{
				add_redirection_from_token(&redirs, el);
				token_list_remove(list, i);
			} else {
				i++;
			}
		}
		return redirs;
	}
	return NULL;
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

TokenList *extract_tokens(TokenList *list, int *i) {
	TokenList *newlist = token_list_init();
	while (*i < list->size && !is_pipe(list, i) && !is_eof(list, i) && !is_semi(list, i)) {
		token_list_add(newlist, list->t[*i]);
		(*i)++;
	}
	return newlist;
}

Node *extract_command_group(TokenList *list, int *i) {
	TokenList *newlist = token_list_init();
	(*i)++; //skip '{'
	while (*i < list->size) {
		token_list_add(newlist, list->t[*i]);
		(*i)++;
	}
	*i = list->size - 1;
	// dprintf(2, "i = %d\n", *i);
	while (!(is_semi(list, &((int){*i - 1})) && is_cmdgrp_end(list, i))) {
		token_list_remove(newlist, *i);
		(*i)--;
	}
	token_list_remove(newlist, *i);

	// printf("============================================\n");
	// tokenListToString(newlist);
	// printf("============================================\n");
	Node *AST = ast_build(newlist);
	AST->tree_tag = TREE_COMMAND_GROUP;
	AST->redirs = eat_redirections(list, TREE_COMMAND_GROUP, *i);
	// if (AST->redirs)
	// 	dprintf(2, "redir size = %d\n", AST->redirs->size);
	// printf("==================AFTER==========================\n");
	// tokenListToString(list);
	// printf("============================================\n");
	return AST;
}

Node *extract_subshell(TokenList *list, int *i) {
	TokenList *newlist = extract_subshell_rec(list, i);
	// printf("============================================\n");
	// tokenListToString(list);
	// printf("============================================\n");
	Node *AST = ast_build(newlist);
	AST->tree_tag = TREE_SUBSHELL;
	AST->redirs = eat_redirections(list, TREE_SUBSHELL, *i);
	// if (AST->redirs)
	// 	dprintf(2, "redir size = %d\n", AST->redirs->size);
	// printf("==================AFTER==========================\n");
	// tokenListToString(list);
	// printf("============================================\n");
	return AST;
	// return ast_build(newlist);
}

bool is_cmdgrp_start(const TokenList *list, const int *i) {
	return (list->t[*i]->tag == T_WORD && !ft_strcmp(list->t[*i]->w_infix, "{"));
}

bool is_cmdgrp_end(const TokenList *list, const int *i) {
	return (list->t[*i]->tag == T_WORD && !ft_strcmp(list->t[*i]->w_infix, "}"));
}

Range is_command_group(const TokenList *list, const int *i) {
	int it_prev = *i - 1;
	int it = *i;
	int it_next = *i + 1;

	Range self = {
		.start = it,
		.end = -1,
	};

	while (it_next < list->size) {
		if (is_cmdgrp_start(list, &it) && (is_ast_operator(list, &it_prev) || is_pipe(list, &it_prev))) {
			it++;
			Range nested = is_command_group(list, &it);
			// dprintf(2, "nested.end = %d\n", nested.end);
			it_prev = nested.end - 1;
			it = nested.end;
			it_next = nested.end + 1;
			if (nested.end == -1) {
				return nested;
			}
			continue;
		}
		if (is_semi(list, &it) && is_cmdgrp_end(list, &it_next)) {
			self.end = it_next;
			return self;
		}
		it_prev++;
		it++;
		it_next++;
	}
	return self;
}

void create_executer_and_push(Executer **executer, TokenList *list, int *i) {
	Executer *new = NULL;

	if (is_cmdgrp_start(list, i)) {
		new = executer_init(extract_command_group(list, i), NULL);
	}
	else if (is_subshell(list, i))
		new = executer_init(extract_subshell(list, i), NULL);
	else
		new = executer_init(NULL, extract_tokens(list, i));

	(*i) += 1; //skip operator
	executer_push_back(executer, new);
}

ExecuterList *build_executer_list(TokenList *list) {
	// if (true){
	// 	printf(C_RED"----------Before EXECUTER-------------"C_RESET"\n");
	// 	tokenListToString(list); //Debug
	// }
	ExecuterList *self = executer_list_init();
	int i = 0;
	while (i < list->size) {
		Executer *executer = NULL;
		type_of_separator next_sep = next_separator(list, &i);

		if (next_sep == S_PIPE) {
			while (next_separator(list, &i) == S_PIPE) {
				create_executer_and_push(&executer, list, &i);
			}
			if (i < list->size) {
				create_executer_and_push(&executer, list, &i);
			}
		}
		if (i < list->size && (next_sep == S_EOF || next_sep == S_SEMI_COLUMN)) {
			create_executer_and_push(&executer, list, &i);
		}
		executer_list_push(self, executer);
	}
	return self;
}

SimpleCommand *parser_parse_current(TokenList *tl, StringList *env) {
	// parser_brace_expansion();
	// parser_tilde_expansion();

	//AU BOULOT
	if (!parser_parameter_expansion(tl, env)){
		return NULL;
	}
	if (!parser_command_substitution(tl, env)) {
		return NULL;
	}
	// parser_arithmetic_expansion();
	// if (!parser_filename_expansion(tl)){
	// 	return NULL;
	// }
	// parser_quote_removal();
	RedirectionList *redirs = parser_get_redirection(tl);
	SimpleCommand *command = parser_get_command(tl);
	command->redir_list = redirs;
	
	return command;
}
