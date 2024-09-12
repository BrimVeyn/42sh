/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_build.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:12:40 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/12 10:13:47 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"
#include "ast.h"

Node *gen_operator_node(TokenList *tok, Node *left, Node *right) {
	Node *self = gc_add(ft_calloc(1, sizeof(Node)), GC_SUBSHELL);
	self->tag = N_OPERATOR;
	self->left = left;
	self->right = right;
	self->value.operator = tok->t[0]->s_type;
	return self;
}

Node *gen_operand_node(TokenList *list) {
	Node *self = gc_add(ft_calloc(1, sizeof(Node)), GC_SUBSHELL);
	self->tag = N_OPERAND;
	self->left = NULL;
	self->right = NULL;
	self->value.operand = list;
	return self;
}

bool is_semi_or_bg(const TokenList *list, const int *it) {
	return is_semi(list, it) || is_bg(list, it);
}

bool is_or_or_and(const TokenList *list, const int *it) {
	return is_or(list, it) || is_and(list, it);
}

bool is_ast_operator(const TokenList *list, const int *it) {
	return is_semi_or_bg(list, it) || is_or_or_and(list, it);
}

TokenList *extract_command(TokenList *list, int *i) {
	TokenList *self = token_list_init();
	while (*i < list->size && !is_ast_operator(list, i)) {
		if (is_subshell(list, i)) {
			skip_subshell(self, list, i);
		}
		if (*i < list->size && !is_ast_operator(list, i)) {
			token_list_add(self, list->t[*i]);
			(*i)++;
		}
	}
	return self;
}

TokenList *extract_operator(TokenList *list, int *i) {
	TokenList *self = token_list_init();
	token_list_add(self, list->t[(*i)++]);
	return self;
}

TokenListStack *split_operator(TokenList *list) {
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

void tokenListToStringAll(TokenListStack *cont) {
	for (uint16_t i = 0; i < cont->size; i++) {
		printf(C_BRIGHT_YELLOW"------------%d---------"C_RESET"\n", i);
		tokenToStringAll(cont->data[i]);
	}
	printf(C_BRIGHT_YELLOW"-----------------------------"C_RESET"\n");
}

bool is_op (TokenList *list) {
	return (list->size == 1 && list->t[0]->tag == T_SEPARATOR);
}

Node *generateTree(TokenListStack *list) {
	NodeStack *self = node_stack_init();
	for (uint16_t i = 0; i < list->size; i++) {
		if (!is_op(list->data[i])) {
			node_stack_push(self, gen_operand_node(list->data[i]));
		} else {
			if (self->size == 1) {
				Node *left = node_stack_pop(self);
				node_stack_push(self, gen_operator_node(list->data[i], left, NULL));
			} else {
				Node *right = node_stack_pop(self);
				Node *left = node_stack_pop(self);
				node_stack_push(self, gen_operator_node(list->data[i], left, right));
			}
		}
	}
	return node_stack_pop(self);
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

Node *ast_build(TokenList *tokens) {
	TokenListStack *branch_stack = split_operator(tokens);
	if (g_debug){
		printf(C_RED"----------BEFORE-------------"C_RESET"\n");
		tokenListToStringAll(branch_stack); //Debug
	}
	TokenListStack *branch_queue = branch_stack_to_rpn(branch_stack);
	// if (g_debug){
	// 	printf(C_RED"----------AFTER-------------"C_RESET"\n");
	// 	tokenListToStringAll(branch_queue); //Debug
	// }
	Node *AST = generateTree(branch_queue);
	// printf("------\n");
	// printTree(AST);
	// printf("-----\n");
	if (g_debug) {
		// gc_cleanup();
		// exit(EXIT_FAILURE);
	}
	return AST;
}

