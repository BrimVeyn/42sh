/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ATokenList.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/21 13:37:19 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/20 16:28:31 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "arithmetic.h"

ATokenList *atoken_list_init(void) {
	ATokenList *self = gc_add(ft_calloc(1, sizeof(ATokenList)), GC_SUBSHELL);
	ATokenList tl = {
		.data = (AToken **) gc_add(ft_calloc(10, sizeof(AToken *)), GC_SUBSHELL),
		.size = 0,
		.capacity = 10,
	};
	*self = tl;
	return self;
}

void atoken_list_add(ATokenList *tl, AToken *token) {
	if (tl->size >= tl->capacity) {
		tl->capacity *= 2;
		AToken **tmp = tl->data;
		tl->data = ft_realloc(tl->data, tl->size, tl->capacity, sizeof(AToken *));
		gc_free(tmp, GC_GENERAL);
		gc_add(tl->data, GC_SUBSHELL);
	}
	tl->data[tl->size] = token;
	tl->size += 1;
}

void atoken_list_add_list(ATokenList *t1, ATokenList *t2) {
	for (uint16_t i = 0; i < t2->size; i++) {
		atoken_list_add(t1, t2->data[i]);
	}
}

void atoken_list_insert(ATokenList *tl, AToken *token, const int index){
	if (tl->size >= tl->capacity) {
		tl->capacity *= 2;
		AToken **tmp = tl->data;
		tl->data = ft_realloc(tl->data, tl->size, tl->capacity, sizeof(AToken *));
		gc_free(tmp, GC_GENERAL);
		gc_add(tl->data, GC_SUBSHELL);
	}
    tl->size += 1;

    for (int i = tl->size - 1; i > index; i--) {
        tl->data[i] = tl->data[i - 1];
    }
    tl->data[index] = token;
}

void atoken_list_insert_list(ATokenList *dest, ATokenList *src, const int index) {
	for (int i = src->size - 1; i >= 0; i--) {
		atoken_list_insert(dest, src->data[i], index);
	}
}

void atoken_list_remove(ATokenList *tl, int index){
    for (int i = index; i < tl->size - 1; i++) {
        tl->data[i] = tl->data[i + 1];
    }
    tl->data[tl->size - 1] = NULL;

    tl->size--;
}
