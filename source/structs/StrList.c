/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StrList.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 16:12:01 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/29 19:19:47 by bvan-pae         ###   ########.fr       */
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

void str_print(Str *node, size_t i) {
	static const char *colors[] = {
		[EXP_WORD] = C_LIGHT_GREEN,
		[EXP_VARIABLE] = C_LIGHT_YELLOW,
		[EXP_ARITHMETIC] = C_LIGHT_BLUE,
		[EXP_CMDSUB] = C_LIGHT_PINK,
		[EXP_SQUOTE] = C_LIGHT_RED,
		[EXP_DQUOTE] = C_DARK_BLUE,
		[EXP_PROC_SUB_IN] = C_LIGHT_BROWN,
		[EXP_PROC_SUB_OUT] = C_LIGHT_ORANGE,
	};

	while (node) {
		dprintf(STDERR_FILENO, "{ [%zu]: %s|%s|"C_RESET" } %s%c%s%c%s%c"C_RESET", ", i, 
		  colors[node->kind], node->str ? node->str : "NULL",
		  colors[EXP_DQUOTE], node->dquote ? 'T' : 'F', 
		  colors[EXP_SQUOTE], node->squote ? 'T' : 'F',
		  colors[EXP_ARITHMETIC], node->file_exp ? 'T' : 'F');
		node = node->next;
	}
}

void str_list_print(const StrList *list) {
	static const char *colors[] = {
		[EXP_WORD] = C_LIGHT_GREEN,
		[EXP_VARIABLE] = C_LIGHT_YELLOW,
		[EXP_ARITHMETIC] = C_LIGHT_BLUE,
		[EXP_CMDSUB] = C_LIGHT_PINK,
		[EXP_SQUOTE] = C_LIGHT_RED,
		[EXP_DQUOTE] = C_DARK_BLUE,
		[EXP_PROC_SUB_IN] = C_LIGHT_BROWN,
		[EXP_PROC_SUB_OUT] = C_LIGHT_ORANGE,
	};
	
	dprintf(STDERR_FILENO, C_LIME"Colors: %sWORD %sCMDSUB, %sARITHMETIC, %sPARAMETER %sDQUOTE %sSQUOTE %sPROCIN %sPROCOUT"C_RESET"\n", 
	colors[EXP_WORD], colors[EXP_CMDSUB], colors[EXP_ARITHMETIC], colors[EXP_VARIABLE], 
	colors[EXP_DQUOTE], colors[EXP_SQUOTE], colors[EXP_PROC_SUB_IN], colors[EXP_PROC_SUB_OUT]);

	dprintf(STDERR_FILENO, C_LIME"String: "C_RESET);
	for (size_t i = 0; i < list->size; i++) {
		Str *node = list->data[i];
		str_print(node, i);
	}
	dprintf(2, "\n");
}
