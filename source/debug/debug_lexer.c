/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_lexer.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 12:53:31 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/04 12:56:54 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

void lexer_debug(Lexer_p lexer) {
	printf("--- Lexer state ---\n");
	printf("Input = |%s|\n", lexer->input);
	printf("Position = %d\n", lexer->position);
	printf("Read_position = %d\n", lexer->read_position);
	printf("Ch = %c | %d\n", lexer->ch, lexer->ch);
	printf("-------------------\n");
}

void tokenToStringAll(TokenList *t) {
	for (uint16_t i = 0; i < t->size; i++) {
		tokenToString(t->t[i], 0);
	}
}

static void printOffset(const size_t offset) {
	for (size_t i = 0; i < offset; i++) {
		printf(" ");
	}
}

void tokenToString(Token *t, size_t offset) {
	printOffset(offset);
	if (t->e != ERROR_NONE) {
		printf(C_RED"ERROR: %s\n"C_RESET, tagName(t->e));
	}
	switch(t->tag) {
		case T_REDIRECTION:
			printf(C_LIGHT_YELLOW"T_REDIRECTION"C_RESET" {\n");
			printOffset(offset + 4);
			printf("Type: "C_MEDIUM_YELLOW"%s\n"C_RESET, tagName(t->r_type));
			tokenToString(t->r_postfix, offset + 4);
			printOffset(offset);
			printf("}\n");
			break;
		case T_NONE:
			printf(C_MEDIUM_GRAY"T_NONE"C_RESET" {}\n");
			break;
		case T_WORD:
			printf(C_MEDIUM_BLUE"T_WORD"C_RESET" {\n");
			printOffset(offset + 4);
			printf(C_LIGHT_BLUE"w_infix: %s\n"C_RESET, t->w_infix);
			printOffset(offset + 4);
			printf(C_LIGHT_BLUE"w_postfix: "C_RESET);
			tokenToString(t->w_postfix, offset + 4);
			printOffset(offset);
			printf("}\n");
			break;
		case T_SEPARATOR:
			printf(C_PURPLE"T_SEPARATOR"C_RESET" {\n");
			printOffset(offset + 4);
			printf("Type: "C_VIOLET"%s\n"C_RESET, tagName(t->s_type));
			break;
		default:
			printf("Format not handled\n");
	}
}
