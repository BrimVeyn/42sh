/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_build_executer_list.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/17 15:09:30 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/17 15:15:16 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

void parser_skip_subshell(TokenList *list, int *j) {
	(*j)++;
	while (!is_end_sub(list, j)) {
		if (is_subshell(list, j)) {
			parser_skip_subshell(list, j);
		}
		(*j)++;
	}
}

void parser_skip_cmdgrp(TokenList *list, int *j) {
	(*j)++;
	while (!is_end_cmdgrp(list, j)) {
		if (is_cmdgrp_start(list, j)) {
			parser_skip_cmdgrp(list, j);
		}
		(*j)++;
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

TokenList *extract_cmdgrp_rec(TokenList *list, int *i) {
	TokenList *newlist = token_list_init();
	(*i)++;

	while (*i < list->size && !is_end_cmdgrp(list, i)) {
		if (*i < list->size && is_cmdgrp_start(list, i)) {
			skip_cmdgrp(newlist, list, i);
		}
		if (*i < list->size) {
			token_list_add(newlist, list->t[*i]);
			(*i)++;
		}
	}
	token_list_remove(newlist, newlist->size - 1);
	(*i) += 1; //skip ';' '}'
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
	// printf("============================================\n");
	// tokenListToString(list);
	// printf("============================================\n");
	TokenList *newlist = extract_cmdgrp_rec(list, i);
	// printf("============================================\n");
	// tokenListToString(newlist);
	// printf("============================================\n");
	// printf("i = %d\n", *i);
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

type_of_separator next_separator(TokenList *list, int *i) {
	int j = *i;
	while (j < list->size) {
		if (is_cmdgrp_start(list, &j)) {
			parser_skip_cmdgrp(list, &j);
		}
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
