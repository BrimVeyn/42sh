/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExecuterList.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:58:41 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/03 17:14:48 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"
#include "ast.h"

ExecuterList *executer_list_init(void) {
	ExecuterList *self = gc_add(ft_calloc(1, sizeof(ExecuterList)));
	self->data = (Executer **) gc_add(ft_calloc(10, sizeof(Executer *)));
	self->size = 0;
	self->capacity = 10;
	return self;
}

void executer_list_push(ExecuterList *tl, Executer *token) {
	if (tl->size >= tl->capacity) {
		tl->capacity *= 2;
		Executer **tmp = tl->data;
		tl->data = (Executer **) ft_realloc(tl->data, tl->size, tl->capacity, sizeof(Executer *));
		gc_free(tmp);
		gc_add(tl->data);
	}
	tl->data[tl->size] = token;
	tl->size += 1;
}

void executer_push_back(Executer **lst, Executer *new_value) {
	Executer *temp;

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

Executer *executer_init(Node *node, TokenList *list) {
	Executer *self = (Executer *) gc_add(ft_calloc(1, sizeof(Executer)));
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
