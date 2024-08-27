/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:08:53 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/27 15:37:35 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "../parser/parser.h"
#include "lexer_enum.h"
#include "lexer_struct.h"
#include <stdint.h>
#include <assert.h>
#include <stdio.h>


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
		case T_GROUPING:
			printf(C_BRONZE"T_COMMAND_GROUPING"C_RESET" {\n");
			printOffset(offset + 4);
			printf("Type: "C_LIGHT_BROWN"%s\n"C_RESET, tagName(t->g_type));
			printOffset(offset + 4);
			printf("CgList: "C_BRONZE"{\n"C_RESET);
			for (uint16_t i = 0; i < t->g_list->size; i++) {
				tokenToString(t->g_list->t[i], offset + 4);
			}
			printOffset(offset + 4);
			printf(C_BRONZE"}\n"C_RESET);
			printOffset(offset + 4);
			printf("CgPostfix:");
			tokenToString(t->g_postfix, offset + 4);
			printOffset(offset);
			printf("}\n"C_RESET);
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

TokenList *lexer_lex_all(Lexer_p l) {
	TokenList *self = token_list_init();
	while (l->ch != '\0') {
		printf("LEXALL: l->ch: %d %c\n", l->ch, l->ch);
		token_list_add(self, lexer_get_next_token(l, false));
	}
	return self;
}

void tokenToStringAll(TokenList *t) {
	for (uint16_t i = 0; i < t->size; i++) {
		tokenToString(t->t[i], 0);
	}
}

#include <readline/readline.h>
#include <readline/history.h>

int main(int ac, char *av[]) {
	//Basic redirection test
	gc_init();

	Token *none_token __attribute__((unused)) = genNoneTok();

	// char *test_input_1 = (char *) gc_add(ft_strdup("10<< file $(salut !)"));
	// (void) test_input_1;
	//
	// Lexer_p l = lexer_init(test_input_1);
	// TokenList *l1 = lexer_lex_all(l);
	// tokenToStringAll(l1);
	
	if (ac == 2 && !ft_strcmp("-i", av[1])) {
		char *input = NULL;
		while ((input = readline("> ")) != NULL) {
			Lexer_p l = lexer_init(input, DEFAULT);
			TokenList *l1 = lexer_lex_all(l);
			tokenToStringAll(l1);
			add_history(input);
			Parser *p = parser_init(input);
			parser_print_state(p);
			parser_parse_all(p);
			parser_print_state(p);
		}
	}


	// TokenList *tl_1 = token_list_init();
	// token_list_add(tl_1, genExprTok(EX_WORD, none_token, "dekodeko", none_token));
	// token_list_add(tl_1, genExprTok(EX_WORD, none_token, "salut", none_token));
	// Token *expected_1 = genCtrlSubTok(CG_CONTROL_SUBSTITUTION, tl_1, none_token);
	// (void) expected_1;
	// Token *got_1 = oneTokenTest(test_input_1);
	// tokenAssert(none_token, got_1, test_input_1, 12);

	gc_cleanup();
}
