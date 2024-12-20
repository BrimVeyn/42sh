/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StrList.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 16:12:01 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/19 17:02:44 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "expansion.h"
#include "colors.h"
#include <stdio.h>
#include <unistd.h>
 
Str *str_init(const ExpKind kind, char *str, bool add_to_gc) {
	Str *self = gc_unique(Str, GC_SUBSHELL);
	self->kind = kind;
	if (add_to_gc)
		self->str = gc(GC_ADD, str, GC_SUBSHELL);
	else 
		self->str = str;
	self->next = NULL;
	return self;
}

void str_list_print(const StrList *list) {
	static const char *colors[] = {
		[EXP_WORD] = C_LIGHT_GREEN,
		[EXP_VARIABLE] = C_LIGHT_YELLOW,
		[EXP_ARITHMETIC] = C_LIGHT_BLUE,
		[EXP_CMDSUB] = C_LIGHT_PINK,
		[EXP_SQUOTE] = C_LIGHT_RED,
		[EXP_DQUOTE] = C_DARK_BLUE,
	};
	
	dprintf(STDERR_FILENO, C_LIME"Colors: %sWORD %sCMDSUB, %sARITHMETIC, %sPARAMETER %sDQUOTE %sSQUOTE"C_RESET"\n", 
	colors[EXP_WORD], colors[EXP_CMDSUB], colors[EXP_ARITHMETIC], colors[EXP_VARIABLE], colors[EXP_DQUOTE], colors[EXP_SQUOTE]);

	dprintf(STDERR_FILENO, C_LIME"String: "C_RESET);
	for (size_t i = 0; i < list->size; i++) {
		Str *node = list->data[i];
		while (node) {
			if (list->data[i]) {
				dprintf(STDERR_FILENO, "{ [%ld]: %s%s"C_RESET" } %s%c%s%c"C_RESET", ", i, colors[node->kind], node->str,
				colors[EXP_DQUOTE], list->data[i]->dquote ? 'T' : 'F', 
				colors[EXP_SQUOTE], list->data[i]->squote ? 'T' : 'F');
			} else {
				dprintf(STDERR_FILENO, "{ "C_LIGHT_YELLOW"NULL"C_RESET" }");
			}
			node = node->next;
		}
	}
	dprintf(2, "\n");
}
