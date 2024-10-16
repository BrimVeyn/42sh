/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:58:41 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/16 17:29:22 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "utils.h"
#include "libft.h"

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
