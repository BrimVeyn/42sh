
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ATokenListStack.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 13:38:32 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/14 20:40:39 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "arithmetic.h"

ATokenStack *atoken_stack_init(void) {
	ATokenStack *self = gc(GC_ADD, ft_calloc(1, sizeof(ATokenStack)), GC_SUBSHELL);
	self->data = (AToken **) gc(GC_ADD, ft_calloc(10, sizeof(AToken *)), GC_SUBSHELL);
	self->size = 0;
	self->capacity = 10;
	return self;
}

AToken *atoken_stack_pop(ATokenStack *self) {
	self->size -= 1;
	return self->data[self->size];
}

void atoken_stack_push(ATokenStack *self, AToken *token) {
	if (self->size >= self->capacity) {
		self->capacity *= 2;
		AToken **tmp = self->data;
		self->data = (AToken **) ft_realloc(self->data, self->size, self->capacity, sizeof(AToken *));
		gc(GC_FREE, tmp, GC_GENERAL);
		gc(GC_ADD, self->data, GC_SUBSHELL);
	}
	self->data[self->size] = token;
	self->size += 1;
}
