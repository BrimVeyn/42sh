/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StrList.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 16:12:01 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/11 16:29:46 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "libft.h"
#include "parser.h"
#include "colors.h"
 
Str *str_init(const ExpKind kind, char *str) {
	Str *self = gc(GC_CALLOC, 1, sizeof(Str), GC_SUBSHELL);
	self->kind = kind;
	self->str = str;
	return self;
}

StrList *str_list_init(void) {
	StrList *self = gc(GC_ADD, ft_calloc(1, sizeof(StrList)), GC_SUBSHELL);
	StrList tl = {
		.data = (Str **) gc(GC_ADD, ft_calloc(10, sizeof(Str *)), GC_SUBSHELL),
		.size = 0,
		.capacity = 10,
	};
	*self = tl;
	return self;
}

void str_list_print(const StrList *list) {
	dprintf(2, C_LIME"String: "C_RESET);
	for (size_t i = 0; i < list->size; i++) {
		if (list->data[i]) {
			dprintf(2, "{ "C_LIGHT_GREEN"%s"C_RESET" }, ", list->data[i]->str);
		} else {
			dprintf(2, "{ "C_LIGHT_YELLOW"NULL"C_RESET" }, ");
		}
	}
	dprintf(2, "\n");
}
