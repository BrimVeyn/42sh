/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_build.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 13:55:55 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/05 09:49:07 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

bool is_operator_ast(Token *tok) {
	return tok->tag == T_SEPARATOR && (tok->s_type == S_AND || tok->s_type == S_OR || tok->s_type == S_EOF);
}

Node *gen_operator_node(TokenList *tok, Node *left, Node *right) {
	Node *self = gc_add(ft_calloc(1, sizeof(Node)));
	self->tag = N_OPERATOR;
	self->left = left;
	self->right = right;
	self->value.operator = tok->t[0]->s_type;
	return self;
}

Node *gen_operand_node(TokenList *list) {
	Node *self = gc_add(ft_calloc(1, sizeof(Node)));
	self->tag = N_OPERAND;
	self->left = NULL;
	self->right = NULL;
	self->value.operand = list;
	return self;
}

TokenList *extract_command(TokenList *list, uint16_t *i) {
	TokenList *self = token_list_init();
	while (*i < list->size && !is_operator_ast(list->t[*i])) {
		if (list->t[*i]->tag == T_SEPARATOR && list->t[*i]->s_type == S_SUB_OPEN) {
			while (*i < list->size && !(list->t[*i]->tag == T_SEPARATOR && list->t[*i]->s_type == S_SUB_CLOSE)) {
				token_list_add(self, list->t[*i]);
				(*i)++;
			}
		}
		if (*i < list->size) {
			token_list_add(self, list->t[*i]);
			(*i)++;
		}
	}
	return self;
}

TokenList *extract_operator(TokenList *list, uint16_t *i) {
	TokenList *self = token_list_init();
	token_list_add(self, list->t[(*i)++]);
	return self;
}

TokenListVector *split_operator(TokenList *list) {
	TokenListVector *self = token_list_vector_init();
	uint16_t i = 0;
	while (i < list->size) {
		token_list_vector_add(self, extract_command(list, &i));
		if (i < list->size && list->t[i]->tag == T_SEPARATOR && list->t[i]->s_type != S_EOF) {
			token_list_vector_add(self, extract_operator(list, &i));
		} else i++;
	}
	return self;
}

void tokenListToStringAll(TokenListVector *cont) {
	for (uint16_t i = 0; i < cont->size; i++) {
		printf("------------%d---------\n", i);
		tokenToStringAll(cont->data[i]);
	}
	printf("-----------------------------\n");
}

bool is_op (TokenList *list) {
	return (list->size == 1 && list->t[0]->tag == T_SEPARATOR);
}

Node *generateTree(TokenListVector *list) {
	NodeStack *self = node_stack_init();
	for (uint16_t i = 0; i < list->size; i++) {
		if (!is_op(list->data[i])) {
			node_stack_push(self, gen_operand_node(list->data[i]));
		} else {
			Node *right = node_stack_pop(self);
			Node *left = node_stack_pop(self);
			node_stack_push(self, gen_operator_node(list->data[i], left, right));
		}
	}
	return node_stack_pop(self);
}

void branch_list_to_rpn(TokenListVector *list) {
	// || and && have same precedence, so fuck it
	for (int i = list->size - 1; i > 1; i-= 2) {
		TokenList *tmp = list->data[i];
		list->data[i] = list->data[i - 1];
		list->data[i - 1] = tmp;
	}
}

Node *ast_build(TokenList *tokens) {
	TokenListVector *branch_list = split_operator(tokens);
	if (g_debug){
		tokenListToStringAll(branch_list); //Debug
	}
	branch_list_to_rpn(branch_list);
	Node *AST = generateTree(branch_list);
	return AST;
}

