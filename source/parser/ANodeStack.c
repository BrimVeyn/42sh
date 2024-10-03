/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ANodeStack.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 15:31:15 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/02 12:49:06 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "arithmetic.h"

ANodeStack *anode_stack_init(void) {
	ANodeStack *self = gc(GC_ADD, ft_calloc(1, sizeof(ANodeStack)), GC_SUBSHELL);
	self->data = (ANode **) gc(GC_ADD, ft_calloc(10, sizeof(ANode *)), GC_SUBSHELL);
	self->size = 0;
	self->capacity = 10;
	return self;
}

ANode *anode_stack_pop(ANodeStack *self) {
	self->size -= 1;
	return self->data[self->size];
}

void anode_stack_push(ANodeStack *tl, ANode *token) {
	if (tl->size >= tl->capacity) {
		tl->capacity *= 2;
		ANode **tmp = tl->data;
		tl->data = (ANode **) ft_realloc(tl->data, tl->size, tl->capacity, sizeof(ANode *));
		gc(GC_FREE, tmp, GC_GENERAL);
		gc(GC_ADD, tl->data, GC_GENERAL);
	}
	tl->data[tl->size] = token;
	tl->size += 1;
}
