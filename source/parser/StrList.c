/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StrList.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 16:12:01 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/15 15:18:19 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "parser.h"
#include "colors.h"
#include <stdio.h>
 
Str *str_init(const ExpKind kind, char *str) {
	Str *self = gc_unique(Str, GC_SUBSHELL);
	self->kind = kind;
	self->str = gc(GC_ADD, str, GC_SUBSHELL);
	self->next = NULL;
	return self;
}

void str_list_print(const StrList *list) {
	static const char *colors[] = {
		[EXP_WORD] = C_LIGHT_GREEN,
		[EXP_VARIABLE] = C_LIGHT_YELLOW,
		[EXP_ARITHMETIC] = C_LIGHT_BLUE,
		[EXP_CMDSUB] = C_LIGHT_PINK,
	};
	
	dprintf(2, C_LIME"Colors: %sWORD %sCMDSUB, %sARITHMETIC, %sVARIABLE"C_RESET"\n", colors[EXP_WORD], colors[EXP_CMDSUB], colors[EXP_ARITHMETIC], colors[EXP_VARIABLE]);
	dprintf(2, C_LIME"String: "C_RESET);
	for (size_t i = 0; i < list->size; i++) {
		Str *node = list->data[i];
		while (node) {
			if (list->data[i]) {
				dprintf(2, "{ %s%s"C_RESET" }, ", colors[node->kind], node->str);
			} else {
				dprintf(2, "{ "C_LIGHT_YELLOW"NULL"C_RESET" }, ");
			}
			node = node->next;
		}
	}
	dprintf(2, "\n");
}
