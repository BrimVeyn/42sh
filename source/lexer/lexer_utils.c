/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 10:19:29 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/02 08:56:31 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

void printOffset(size_t offset) {
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

//Generate generic empty token
Token *genNoneTok(void) {
	Token *tok = (Token *) gc_add(ft_calloc(1, sizeof(Token)));
	Token self = {
		.tag = T_NONE,
	};
	*tok = self;
	return tok;
}

void tokenListToString(TokenList *tl) {
	for (uint16_t i = 0; i < tl->size; i++) {
		tokenToString(tl->t[i], 0);
	}
}


void tokenToStringAll(TokenList *t) {
	for (uint16_t i = 0; i < t->size; i++) {
		tokenToString(t->t[i], 0);
	}
}

bool is_fdable_redirection(Lexer_p l) {
	const char *input_ptr = &l->input[l->position];
	if (!ft_strncmp(input_ptr, ">&", 2) ||
		!ft_strncmp(input_ptr, "<&", 2) ||
		ft_strchr("<>", l->ch)) {
		return true;
	}
	return false;
}
