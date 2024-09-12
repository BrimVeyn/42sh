/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NodeStack.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 15:31:15 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/10 10:13:08 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/42sh.h"

NodeStack *node_stack_init(void) {
	NodeStack *self = gc_add(ft_calloc(1, sizeof(NodeStack)), GC_SUBSHELL);
	self->data = (Node **) gc_add(ft_calloc(10, sizeof(Node *)), GC_SUBSHELL);
	self->size = 0;
	self->capacity = 10;
	return self;
}

Node *node_stack_pop(NodeStack *self) {
	self->size -= 1;
	return self->data[self->size];
}

void node_stack_push(NodeStack *tl, Node *token) {
	if (tl->size >= tl->capacity) {
		tl->capacity *= 2;
		Node **tmp = tl->data;
		tl->data = (Node **) ft_realloc(tl->data, tl->size, tl->capacity, sizeof(Node *));
		gc_free(tmp, GC_GENERAL);
		gc_add(tl->data, GC_GENERAL);
	}
	tl->data[tl->size] = token;
	tl->size += 1;
}
