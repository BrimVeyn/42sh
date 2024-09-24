/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_build.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:12:40 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/24 09:31:56 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "debug.h"

void skip_cmdgrp(TokenList *self, TokenList *list, int *i) {
	token_list_add(self, list->t[*i]);
	(*i)++;

	while (*i < list->size && !is_end_cmdgrp(list, i)) {
		if (is_cmdgrp_start(list, i))
			skip_cmdgrp(self, list, i);
		if (*i < list->size) {
			token_list_add(self, list->t[*i]);
			(*i)++;
		}
	}
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

TokenList *extract_operator(TokenList *list, int *i) {
	TokenList *self = token_list_init();
	token_list_add(self, list->t[(*i)++]);
	return self;
}

TokenList *extract_command(TokenList *list, int *i) {
	// dprintf(2, "-------------START----------\n");
	// dprintf(2, "i = %d\n", *i);
	// tokenListToString(list);
	// dprintf(2, "----------------------------\n");
	TokenList *self = token_list_init();
	while (*i < list->size && !is_ast_operator(list, i)) {
		if (is_cmdgrp_start(list , i)) {
			skip_cmdgrp(self, list, i);
			continue;
		}
		if (is_subshell(list, i)) {
			skip_subshell(self, list, i);
			continue;
		}
		if (*i < list->size && !is_ast_operator(list, i) && !is_eof(list, i)) {
			token_list_add(self, list->t[*i]);
		}
		(*i)++;
	}
	// dprintf(2, "-------------AFTER EXTRACT COMMAND----------\n");
	// tokenListToString(self);
	// dprintf(2, "----------------------------\n");
	return self;
}

TokenListStack *split_operator(TokenList *list) {
	// dprintf(2, "-------------PLOUF----------\n");
	// tokenListToString(list);
	// dprintf(2, "----------------------------\n");
	TokenListStack *self = token_list_stack_init();
	int i = 0;
	while (i < list->size) {
		if (is_eof(list,&i)) {
			i++;
			continue;
		}
		token_list_stack_push(self, extract_command(list, &i));
		if (i < list->size && is_ast_operator(list, &i)) {
			token_list_stack_push(self, extract_operator(list, &i));
		} else i++;
	}
	return self;
}


// || and && have same precedence {1}
// & and ; have same precedence {2}
// {1} has higher precedence than {2}

bool has_higher_precedence(TokenListStack *operator, TokenList *current) {
	if (operator->size == 0) return true;
	const TokenList *last_el = operator->data[operator->size - 1];
	const int zero = 0;

	if (is_or_or_and(current, &zero) && is_semi_or_bg(last_el, &zero))
		return true;
	return false;
}

TokenListStack *branch_stack_to_rpn(TokenListStack *list) {
	TokenListStack *output = token_list_stack_init();
	TokenListStack *operator = token_list_stack_init();
	const int zero = 0;
	
	for (int i = 0; i < list->size; i++) {
		TokenList *current = list->data[i];
		if (is_ast_operator(current, &zero)) {
			while(!has_higher_precedence(operator, current)) {
				token_list_stack_push(output, token_list_stack_pop(operator));
			}
			token_list_stack_push(operator, current);
		} else {
			token_list_stack_push(output, current);
		}
	}
	while (operator->size > 0) {
		token_list_stack_push(output, token_list_stack_pop(operator));
	}
	return output;
}

#include <stdio.h>
#include "debug.h"
#include "colors.h"

Node *ast_build(TokenList *tokens) {
	// printf("~~~~~~~~~~~~~~~~~~~~~~~~\n");
	// tokenListToString(tokens);
	TokenListStack *branch_stack = split_operator(tokens);
	// dprintf(2, C_RED"----------BEFORE-------------"C_RESET"\n");
	// for (size_t i = 0; i < branch_stack->size; i++) {
	// 	dprintf(2, "--------%zu------\n", i);
	// 	tokenListToString(branch_stack->data[i]);
	// 	dprintf(2, "-----------------\n");
	// }
	TokenListStack *branch_queue = branch_stack_to_rpn(branch_stack);
	// for (size_t i = 0; i < branch_queue->size; i++) {
	// 	dprintf(2, "--------%zu------\n", i);
	// 	tokenListToString(branch_queue->data[i]);
	// 	dprintf(2, "-----------------\n");
	// }
	Node *AST = generate_tree(branch_queue);
	// printf("------\n");
	// printTree(AST);
	// printf("-----\n");
	if (g_debug) {
		// gc_cleanup();
		// exit(EXIT_FAILURE);
	}
	return AST;
}

