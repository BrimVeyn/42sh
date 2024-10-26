/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_lexer.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 12:53:31 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/26 15:23:16 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "colors.h"
#include "lexer.h"
#include "libft.h"
#include "utils.h"

#include <stdio.h>

char *get_line_x(char *in, const size_t n) {
	char *line_ptr = in;
	for (size_t i = 0; i < n; i++) {
		line_ptr = ft_strchr(line_ptr, '\n');
	}

	char *end = ft_strchr(line_ptr, '\n');
	char *return_line = ft_substr(in, (line_ptr - in), (end - line_ptr));
	return return_line;
}

void lexer_debug(Lexer_p lexer) {
	dprintf(2, "--- Lexer state ---\n");
	dprintf(2, "Input = |%s|\n", lexer->input);
	dprintf(2, "Position = %zu\n", lexer->position);
	dprintf(2, "Read_position = %zu\n", lexer->read_position);
	dprintf(2, "Ch = %c | %d\n", lexer->ch, lexer->ch);
	dprintf(2, "-------------------\n");
}

void tokenListToString(TokenList *t) {
	for (uint16_t i = 0; i < t->size; i++) {
		tokenToString(t->data[i], 0);
	}
}

static void printOffset(const size_t offset) {
	for (size_t i = 0; i < offset; i++) {
		dprintf(2, " ");
	}
}

void tokenToString(Token *t, size_t offset) {
	printOffset(offset);
	if (t->e != ERROR_NONE) {
		dprintf(2, C_RED"ERROR: %s\n"C_RESET, tagName(t->e));
	}
	switch(t->tag) {
		case T_REDIRECTION:
			dprintf(2, C_LIGHT_YELLOW"T_REDIRECTION"C_RESET" {\n");
			printOffset(offset + 4);
			dprintf(2, "Type: "C_MEDIUM_YELLOW"%s\n"C_RESET, tagName(t->r_type));
			tokenToString(t->r_postfix, offset + 4);
			printOffset(offset);
			dprintf(2, "}\n");
			break;
		case T_NONE:
			dprintf(2, C_MEDIUM_GRAY"T_NONE"C_RESET" {}\n");
			break;
		case T_WORD:
			dprintf(2, C_MEDIUM_BLUE"T_WORD"C_RESET" {\n");
			printOffset(offset + 4);
			dprintf(2, C_LIGHT_BLUE"w_infix: %s\n"C_RESET, t->w_infix);
			printOffset(offset + 4);
			dprintf(2, C_LIGHT_BLUE"w_postfix: "C_RESET);
			tokenToString(t->w_postfix, offset + 4);
			printOffset(offset);
			dprintf(2, "}\n");
			break;
		case T_SEPARATOR:
			dprintf(2, C_PURPLE"T_SEPARATOR"C_RESET" {\n");
			printOffset(offset + 4);
			dprintf(2, "Type: "C_VIOLET"%s\n"C_RESET, tagName(t->s_type));
			printOffset(offset);
			dprintf(2, "}\n");
			break;
		default:
			dprintf(2, "Format not handled\n");
	}
}
