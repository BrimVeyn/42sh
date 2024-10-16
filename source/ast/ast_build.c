/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_build.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:12:40 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/14 13:53:11 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "lexer.h"
#include "utils.h"

void skip_cmdgrp(TokenList *self, TokenList *list, size_t *i) {
	da_push(self, list->data[*i]);
	(*i)++;

	while (*i < list->size && !is_end_cmdgrp(list, i)) {
		if (is_cmdgrp_start(list, i))
			skip_cmdgrp(self, list, i);
		if (*i < list->size) {
			da_push(self, list->data[*i]);
			(*i)++;
		}
	}
}

void skip_subshell(TokenList *newlist, TokenList *list, size_t *i) {
	da_push(newlist, list->data[(*i)++]);

	while (*i < list->size && !is_end_sub(list, i)) {
		if (is_subshell(list, i))
			skip_subshell(newlist, list, i);
		if (*i < list->size)
			da_push(newlist, list->data[(*i)++]);
	}
}

TokenList *extract_operator(TokenList *list, size_t *i) {
	da_create(self, TokenList, sizeof(Token *), GC_SUBSHELL);
	da_push(self, list->data[(*i)++]);
	return self;
}

TokenList *extract_command(TokenList *list, size_t *i) {
	da_create(self, TokenList, sizeof(Token *), GC_SUBSHELL);

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
			da_push(self, list->data[*i]);
		}
		(*i)++;
	}
	return self;
}

TokenListStack *split_operator(TokenList *list) {
	da_create(self, TokenListStack, sizeof(TokenList *), GC_SUBSHELL);

	size_t i = 0;
	while (i < list->size) {
		if (is_eof(list,&i)) {
			i++;
			continue;
		}
		da_push(self, extract_command(list, &i));
		if (i < list->size && is_ast_operator(list, &i)) {
			da_push(self, extract_operator(list, &i));
		} else 
			i++;
	}
	return self;
}


// || and && have same precedence {1}
// & and ; have same precedence {2}
// {1} has higher precedence than {2}

bool has_higher_precedence(TokenListStack *operator, TokenList *current) {
	if (operator->size == 0) return true;
	const TokenList *last_el = operator->data[operator->size - 1];
	const size_t zero = 0;

	if (is_or_or_and(current, &zero) && is_semi_or_bg(last_el, &zero))
		return true;
	return false;
}

TokenListStack *branch_stack_to_rpn(TokenListStack *list) {
	da_create(output, TokenListStack, sizeof(TokenList *), GC_SUBSHELL);
	da_create(operator, TokenListStack, sizeof(TokenList *), GC_SUBSHELL);
	const size_t zero = 0;
	
	for (size_t i = 0; i < list->size; i++) {
		TokenList *current = list->data[i];
		if (is_ast_operator(current, &zero)) {
			while(!has_higher_precedence(operator, current))
				da_push(output, da_pop(operator));
			da_push(operator, current);
		} else
			da_push(output, current);
	}
	while (operator->size)
		da_push(output, da_pop(operator));
	return output;
}

Node *ast_build(TokenList *tokens) {
	TokenListStack *branch_stack = split_operator(tokens);
	TokenListStack *branch_queue = branch_stack_to_rpn(branch_stack);
	return generate_tree(branch_queue);
}

