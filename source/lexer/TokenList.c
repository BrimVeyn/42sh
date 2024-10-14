/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TokenList.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/21 13:37:19 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/14 13:47:32 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "utils.h"

void token_list_add_list(TokenList *t1, TokenList *t2) {
	for (uint16_t i = 0; i < t2->size; i++) {
		da_push(t1, t2->data[i]);
	}
}

void token_list_insert(TokenList *tl, Token *token, const int index){
	if (tl->size >= tl->capacity) {
		tl->capacity *= 2;
		tl->data = gc(GC_REALLOC, tl->data, tl->size, tl->capacity, sizeof(void *), GC_SUBSHELL);
	}
    tl->size += 1;

    for (int i = tl->size - 1; i > index; i--) {
        tl->data[i] = tl->data[i - 1];
    }
    tl->data[index] = token;
}

void token_list_insert_list(TokenList *dest, TokenList *src, const int index) {
	for (int i = src->size - 1; i >= 0; i--) {
		token_list_insert(dest, src->data[i], index);
	}
}

void token_list_remove(TokenList *tl, int index){
    for (size_t i = index; i < tl->size - 1; i++) {
        tl->data[i] = tl->data[i + 1];
    }
    tl->data[tl->size - 1] = NULL;

    tl->size--;
}
