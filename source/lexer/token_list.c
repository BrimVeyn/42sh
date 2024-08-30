/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_list.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/21 13:37:19 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/30 15:16:10 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

TokenList *token_list_init(void) {
	TokenList *self = gc_add(ft_calloc(1, sizeof(TokenList)));
	TokenList tl = {
		.t = (Token **) gc_add(ft_calloc(10, sizeof(Token *))),
		.size = 0,
		.capacity = 10,
	};
	*self = tl;
	return self;
}

void token_list_add(TokenList *tl, Token *token) {
	if (tl->size >= tl->capacity) {
		tl->capacity *= 2;
		tl->t = ft_realloc(tl->t, tl->size, tl->capacity, sizeof(Token *));
	}
	tl->t[tl->size] = token;
	tl->size += 1;
}
