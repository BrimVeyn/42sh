/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_node.init.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/17 14:54:24 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/14 17:02:23 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "libft.h"
#include "utils.h"

Node *gen_operator_node(TokenList *tok, Node *left, Node *right) {
	Node *self = gc(GC_ADD, ft_calloc(1, sizeof(Node)), GC_SUBSHELL);
	self->tag = N_OPERATOR;
	self->tree_tag = TREE_DEFAULT;
	self->redirs = NULL;
	self->left = left;
	self->right = right;
	self->value.operator = tok->data[0]->s_type;
	return self;
}

Node *gen_operand_node(TokenList *list) {
	Node *self = gc(GC_ADD, ft_calloc(1, sizeof(Node)), GC_SUBSHELL);
	self->tag = N_OPERAND;
	self->tree_tag = TREE_DEFAULT;
	self->redirs = NULL;
	self->left = NULL;
	self->right = NULL;
	self->value.operand = list;
	return self;
}

bool is_op (TokenList *list) {
	return (list->size == 1 && list->data[0]->tag == T_SEPARATOR);
}

Node *generate_tree(TokenListStack *list) {
	da_create(self, NodeStack, sizeof(Node *), GC_SUBSHELL);
	for (uint16_t i = 0; i < list->size; i++) {
		if (!is_op(list->data[i])) {
			da_push(self, gen_operand_node(list->data[i]));
		} else {
			if (self->size == 1) {
				Node *left = da_pop(self);
				da_push(self, gen_operator_node(list->data[i], left, NULL));
			} else {
				Node *right = da_pop(self);
				Node *left = da_pop(self);
				da_push(self, gen_operator_node(list->data[i], left, right));
			}
		}
	}
	return da_pop(self);
}
