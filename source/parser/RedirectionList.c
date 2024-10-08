/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RedirectionList.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 15:32:25 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/02 12:49:06 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "libft.h"
#include "utils.h"

RedirectionList *redirection_list_init(void) {
	RedirectionList *self = gc(GC_ADD, ft_calloc(1, sizeof(RedirectionList)), GC_SUBSHELL);
	RedirectionList rl = {
		.r = (Redirection **) gc(GC_ADD, ft_calloc(10, sizeof(Redirection *)), GC_SUBSHELL),
		.size = 0,
		.capacity = 10,
	};
	*self = rl;
	return self;
}

void redirection_list_add(RedirectionList *rl, Redirection *redirection) {
	if (rl->size >= rl->capacity) {
		rl->capacity *= 2;
		Redirection **tmp = rl->r;
		rl->r = ft_realloc(rl->r, rl->size, rl->capacity, sizeof(Redirection *));
		gc(GC_FREE, tmp, GC_GENERAL);
		gc(GC_ADD, rl->r, GC_GENERAL);
	}
	rl->r[rl->size] = redirection;
	rl->size += 1;
}

void redirection_list_prepend(RedirectionList *rl, Redirection *redirection){
	if (rl->size >= rl->capacity) {
		rl->capacity *= 2;
		Redirection **tmp = rl->r;
		rl->r = ft_realloc(rl->r, rl->size, rl->capacity, sizeof(Redirection *));
		gc(GC_FREE, tmp, GC_GENERAL);
		gc(GC_ADD, rl->r, GC_GENERAL);
	}
	rl->size += 1;
	for (int i = rl->size; i > 0; i--){
		rl->r[i] = rl->r[i - 1];
	}
	rl->r[0] = redirection;
}
