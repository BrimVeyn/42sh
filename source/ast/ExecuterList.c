/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExecuterList.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:58:41 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/07 10:19:18 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "utils.h"
#include "libft.h"

ExecuterList *executer_list_init(void) {
	ExecuterList *self = gc(GC_ADD, ft_calloc(1, sizeof(ExecuterList)), GC_SUBSHELL);
	self->data = (Process **) gc(GC_ADD, ft_calloc(10, sizeof(Job *)), GC_SUBSHELL);
	self->size = 0;
	self->capacity = 10;
	return self;
}

void executer_list_push(ExecuterList *tl, Process *process) {
	if (tl->size >= tl->capacity) {
		tl->capacity *= 2;
		Process **tmp = tl->data;
		tl->data = (Process **) ft_realloc(tl->data, tl->size, tl->capacity, sizeof(Process *));
		gc(GC_FREE, tmp, GC_GENERAL);
		gc(GC_ADD, tl->data, GC_GENERAL);
	}
	tl->data[tl->size] = process;
	tl->size += 1;
}

void process_push_back(Process **lst, Process *new_value) {
	Process *temp;

	if (*lst == NULL)
		*lst = new_value;
	else
	{
		temp = *lst;
		while (temp->next)
			temp = temp->next;
		temp->next = new_value;
	}
}

Process *process_init(Node *node, TokenList *list) {
	Process *self = (Process *) gc(GC_CALLOC, 1, sizeof(Process), GC_SUBSHELL);
	if (node != NULL) {
		self->data_tag = DATA_NODE;
		self->n_data = node;
	}
	if (list != NULL) {
		self->data_tag = DATA_TOKENS,
		self->s_data = list;
	}
	return self;
}
