/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_build_executer_list.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/17 15:09:30 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/16 17:03:56 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"

#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

void parser_skip_subshell(TokenList *list, size_t *j) {
	(*j)++;
	while (!is_end_sub(list, j)) {
		if (is_subshell(list, j)) {
			parser_skip_subshell(list, j);
		}
		(*j)++;
	}
}

void parser_skip_cmdgrp(TokenList *list, size_t *j) {
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
		size_t i = start;

		if (i >= list->size) 
			return NULL;

		da_create(redirs, RedirectionList, sizeof(Redirection *), GC_SUBSHELL);

		while (i < list->size) {
			const Token *el = list->data[i];
			if (is_redirection(list, &i))
			{
				add_redirection_from_token(redirs, el);
				token_list_remove(list, i);
			} else {
				i++;
			}
		}
		return redirs;
	}
	return NULL;
}

TokenList *extract_subshell_rec(TokenList *list, size_t *i) {
	da_create(newlist, TokenList, sizeof(Token *), GC_SUBSHELL);
	(*i)++;

	while (*i < list->size && !is_end_sub(list, i)) {
		if (*i < list->size && is_subshell(list, i)) {
			skip_subshell(newlist, list, i);
		}
		if (*i < list->size) {
			da_push(newlist, list->data[*i]);
			(*i)++;
		}
	}
	(*i) += 1;
	return newlist;
}

TokenList *extract_cmdgrp_rec(TokenList *list, size_t *i) {
	da_create(newlist, TokenList, sizeof(Token *), GC_SUBSHELL);
	(*i)++;

	while (*i < list->size && !is_end_cmdgrp(list, i)) {
		if (*i < list->size && is_cmdgrp_start(list, i)) {
			skip_cmdgrp(newlist, list, i);
		}
		if (*i < list->size) {
			da_push(newlist, list->data[*i]);
			(*i)++;
		}
	}
	token_list_remove(newlist, newlist->size - 1);
	(*i) += 1; //skip ';' '}'
	return newlist;
}

TokenList *extract_tokens(TokenList *list, size_t *i) {
	da_create(newlist, TokenList, sizeof(Token *), GC_SUBSHELL);
	while (*i < list->size && !is_pipe(list, i) && !is_eof(list, i) && !is_semi(list, i)) {
		da_push(newlist, list->data[*i]);
		(*i)++;
	}
	return newlist;
}

Node *extract_command_group(TokenList *list, size_t *i) {
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


Node *extract_subshell(TokenList *list, size_t *i) {
	// printf("ici !\n");
	TokenList *newlist = extract_subshell_rec(list, i);
	// printf("============================================\n");
	// tokenListToString(newlist);
	// printf("============================================\n");
	if (newlist->size == 0)
		return NULL;
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

void create_executer_and_push(Process **executer, TokenList *list, size_t *i) {
	Process *new = NULL;

	if (is_cmdgrp_start(list, i)) {
		new = process_init(extract_command_group(list, i), NULL);
	}
	else if (is_subshell(list, i))
		new = process_init(extract_subshell(list, i), NULL);
	else
		new = process_init(NULL, extract_tokens(list, i));

	(*i) += 1; //skip operator
	process_push_back(executer, new);
}

type_of_separator next_separator(TokenList *list, size_t *i) {
	size_t j = *i;
	while (j < list->size) {
		if (is_cmdgrp_start(list, &j)) {
			parser_skip_cmdgrp(list, &j);
		}
		if (is_subshell(list, &j)) {
			parser_skip_subshell(list, &j);
		}
		if (is_semi(list, &j) || is_eof(list, &j) || is_pipe(list, &j)) {
			return list->data[j]->s_type;
		}
		(j)++;
	}
	return S_EOF;
}

#include <fcntl.h>

Process *build_executer_list(TokenList *token_list) {
	// if (true){
	// 	printf(C_RED"----------Before EXECUTER-------------"C_RESET"\n");
	// 	tokenListToString(token_list); //Debug
	// }
	size_t i = 0;
	Process *process_list = NULL;
	type_of_separator next_sep = next_separator(token_list, &i);

	if (next_sep == S_PIPE) {
		while (next_separator(token_list, &i) == S_PIPE) {
			create_executer_and_push(&process_list, token_list, &i);
		}
		if (i < token_list->size) {
			create_executer_and_push(&process_list, token_list, &i);
		}
	}
	if (i < token_list->size && (next_sep == S_EOF || next_sep == S_SEMI_COLUMN)) {
		create_executer_and_push(&process_list, token_list, &i);
	}

	return process_list;
}
