/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TokenListVector.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 13:38:32 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/04 12:52:03 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

TokenListVector *token_list_vector_init(void) {
	TokenListVector *self = gc_add(ft_calloc(1, sizeof(TokenListVector)));
	self->data = (TokenList **) gc_add(ft_calloc(10, sizeof(TokenList *)));
	self->size = 0;
	self->capacity = 10;
	return self;
}

void token_list_vector_add(TokenListVector *tl, TokenList *token) {
	if (tl->size >= tl->capacity) {
		tl->capacity *= 2;
		TokenList **tmp = tl->data;
		tl->data = ft_realloc(tl->data, tl->size, tl->capacity, sizeof(TokenList *));
		gc_free(tmp);
		gc_add(tl->data);
	}
	tl->data[tl->size] = token;
	tl->size += 1;
}
