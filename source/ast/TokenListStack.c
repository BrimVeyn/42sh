/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TokenListStack.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 13:38:32 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/14 20:40:39 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "libft.h"
#include "utils.h"

TokenListStack *token_list_stack_init(void) {
	TokenListStack *self = gc_add(ft_calloc(1, sizeof(TokenListStack)), GC_SUBSHELL);
	self->data = (TokenList **) gc_add(ft_calloc(10, sizeof(TokenList *)), GC_SUBSHELL);
	self->size = 0;
	self->capacity = 10;
	return self;
}

TokenList *token_list_stack_pop(TokenListStack *self) {
	self->size -= 1;
	return self->data[self->size];
}

void token_list_stack_push(TokenListStack *self, TokenList *token) {
	if (self->size >= self->capacity) {
		self->capacity *= 2;
		TokenList **tmp = self->data;
		self->data = (TokenList **) ft_realloc(self->data, self->size, self->capacity, sizeof(TokenList *));
		gc_free(tmp, GC_GENERAL);
		gc_add(self->data, GC_SUBSHELL);
	}
	self->data[self->size] = token;
	self->size += 1;
}

