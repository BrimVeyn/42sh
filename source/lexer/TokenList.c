/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TokenList.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/21 13:37:19 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/11 11:09:30 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

TokenList *token_list_init(void) {
	TokenList *self = gc_add(ft_calloc(1, sizeof(TokenList)), GC_SUBSHELL);
	TokenList tl = {
		.t = (Token **) gc_add(ft_calloc(10, sizeof(Token *)), GC_SUBSHELL),
		.size = 0,
		.capacity = 10,
	};
	*self = tl;
	return self;
}

void token_list_add(TokenList *tl, Token *token) {
	if (tl->size >= tl->capacity) {
		tl->capacity *= 2;
		Token **tmp = tl->t;
		tl->t = ft_realloc(tl->t, tl->size, tl->capacity, sizeof(Token *));
		gc_free(tmp, GC_GENERAL);
		gc_add(tl->t, GC_SUBSHELL);
	}
	tl->t[tl->size] = token;
	tl->size += 1;
}

void token_list_add_list(TokenList *t1, TokenList *t2) {
	for (uint16_t i = 0; i < t2->size; i++) {
		token_list_add(t1, t2->t[i]);
	}
}

void token_list_insert(TokenList *tl, Token *token, int index){
	if (tl->size >= tl->capacity) {
		tl->capacity *= 2;
		Token **tmp = tl->t;
		tl->t = ft_realloc(tl->t, tl->size, tl->capacity, sizeof(Token *));
		gc_free(tmp, GC_GENERAL);
		gc_add(tl->t, GC_SUBSHELL);
	}
    tl->size += 1;

    for (int i = tl->size - 1; i > index; i--) {
        tl->t[i] = tl->t[i - 1];
    }
    tl->t[index] = token;
}

void token_list_remove(TokenList *tl, int index){

    // if (index < 0 || index >= tl->size) {
    //     fprintf(stderr, "Invalid index: %d\n", index);
    //     return;
    // }

    for (int i = index; i < tl->size - 1; i++) {
        tl->t[i] = tl->t[i + 1];
    }

    tl->t[tl->size - 1] = NULL;

    tl->size--;
}
