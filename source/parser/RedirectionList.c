/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RedirectionList.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 15:32:25 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/04 13:08:39 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

RedirectionList *redirection_list_init(void) {
	RedirectionList *self = gc_add(ft_calloc(1, sizeof(RedirectionList)));
	RedirectionList rl = {
		.r = (Redirection **) gc_add(ft_calloc(10, sizeof(Redirection *))),
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
		gc_free(tmp);
		gc_add(rl->r);
	}
	rl->r[rl->size] = redirection;
	rl->size += 1;
}

void redirection_list_prepend(RedirectionList *rl, Redirection *redirection){
	if (rl->size >= rl->capacity) {
		rl->capacity *= 2;
		Redirection **tmp = rl->r;
		rl->r = ft_realloc(rl->r, rl->size, rl->capacity, sizeof(Redirection *));
		gc_free(tmp);
		gc_add(rl->r);
	}
	rl->size += 1;
	for (int i = rl->size; i > 0; i--){
		rl->r[i] = rl->r[i - 1];
	}
	rl->r[0] = redirection;
}