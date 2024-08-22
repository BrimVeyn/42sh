/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:08:53 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/22 16:32:48 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "lexer_enum.h"
#include "lexer_struct.h"
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

bool tokenCmp(Token *expected, Token *got) {
	if (expected->tag != got->tag) return false;

	switch(expected->tag) {
		case T_END_OF_FILE:
			return true;
		case T_REDIRECTION:
			if (expected->r_type != got->r_type)
				return false;
			if (!tokenCmp(expected->r_postfix, got->r_postfix))
				return false;
			break;
		case T_NONE:
			return true;
		case T_EXPRESSION:
			if (expected->ex_type != got->ex_type)
				return false;
			if (!tokenCmp(expected->ex_prefix, got->ex_prefix))
				return false;
			if (!got->ex_infix || strcmp(expected->ex_infix, got->ex_infix))
				return false;
			if (!tokenCmp(expected->ex_postfix, got->ex_postfix))
				return false;
			break;
		case T_COMMAND_GROUPING:
			if (expected->cg_type != got->cg_type)
				return false;
			if (expected->cg_list->size != got->cg_list->size)
				return false;
			for (uint16_t i = 0; i < expected->cg_list->size; i++) {
				if (!tokenCmp(expected->cg_list->t[i], got->cg_list->t[i]))
					return false;
			}
			return tokenCmp(expected->cg_postfix, got->cg_postfix);
			break;
		default:
			printf("Missing comparison here !\n");
			return false;
	}
	return true;
}

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
		case T_END_OF_FILE:
			printf(C_MEDIUM_GRAY"T_EOF"C_RESET" {}\n");
			break;
		case T_EXPRESSION:
			printf(C_MEDIUM_CYAN"T_EXPRESSION"C_RESET" {\n");
			printOffset(offset + 4);
			printf("Type: "C_LIGHT_CYAN"%s\n"C_RESET, tagName(t->ex_type));
			printOffset(offset + 4);
			printf("ExPrefix:");
			tokenToString(t->ex_prefix, offset + 4);
			printOffset(offset + 4);
			printf("ExInfix:"C_PEACH" |%s|"C_RESET"\n", t->ex_infix);
			printOffset(offset + 4);
			printf("ExPostfix:");
			tokenToString(t->ex_postfix, offset + 4);
			printOffset(offset);
			printf("}\n");
			break;
		case T_COMMAND_GROUPING:
			printf(C_BRONZE"T_COMMAND_GROUPING"C_RESET" {\n");
			printOffset(offset + 4);
			printf("Type: "C_LIGHT_BROWN"%s\n"C_RESET, tagName(t->cg_type));
			printOffset(offset + 4);
			printf("CgList: "C_BRONZE"{\n"C_RESET);
			for (uint16_t i = 0; i < t->cg_list->size; i++) {
				tokenToString(t->cg_list->t[i], offset + 4);
			}
			printOffset(offset + 4);
			printf(C_BRONZE"}\n"C_RESET);
			printOffset(offset + 4);
			printf("CgPostfix:");
			tokenToString(t->cg_postfix, offset + 4);
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


//Generate expr token
Token *genExprTok(type_of_expression type, Token *prefix, char *infix, Token *postfix) {
	Token *tok = (Token *) gc_add(ft_calloc(1, sizeof(Token)));
	Token self = {
		.tag = T_EXPRESSION,
		.ex_type = type,
		.ex_prefix = prefix,
		.ex_infix = infix,
		.ex_postfix = postfix,
	};
	*tok = self;
	return tok;
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

Token *oneTokenTest(char *input) {
	Lexer_p l = lexer_init(input);

	Token *tok = lexer_get_next_token(l);
	return tok;
}

//Custom assert to check whether two tokens are equal, prints debug if they're not
void tokenAssert(Token *expected, Token *got, char *input, int id) {
	if (!tokenCmp(expected, got)) {
		printf("Input: "C_YELLOW"%s\n"C_RESET, input);
		printf(C_GREEN"EXPECTED :\n"C_RESET);
		tokenToString(expected, 0);
		printf(C_LIGHT_RED"GOT :\n"C_RESET);
		tokenToString(got, 0);
	} else {
		printf(C_LIGHT_GREEN"Test %d passed !\n"C_RESET, id);
	}
}

Token *genEOFTok(void) {
	Token *self = gc_add(ft_calloc(1, sizeof(Token)));
	Token tok = {
		.tag = T_END_OF_FILE,
	};
	*self = tok;
	return self;
}

// Token *genCtrlSubTok(type_of_command_grouping type, TokenList *tl, Token *postfix) {
// 	Token *self = gc_add(ft_calloc(1, sizeof(Token)));
// 	Token tok = {
// 		.tag = T_COMMAND_GROUPING,
// 		.cg_type = type,
// 		.cg_list = tl,
// 		.cg_postfix = postfix,
// 	};
// 	*self = tok;
// 	return self;
// }

void tokenListToString(TokenList *tl) {
	for (uint16_t i = 0; i < tl->size; i++) {
		tokenToString(tl->t[i], 0);
	}
}

TokenList *lexer_lex_all(Lexer_p l) {
	TokenList *self = token_list_init();
	while (l->ch != '\0') {
		printf("LEXALL: l->ch: %d %c\n", l->ch, l->ch);
		token_list_add(self, lexer_get_next_token(l));
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
	Token *eof_token __attribute__((unused)) = genEOFTok();

	// char *test_input_1 = (char *) gc_add(ft_strdup("10<< file $(salut !)"));
	// (void) test_input_1;
	//
	// Lexer_p l = lexer_init(test_input_1);
	// TokenList *l1 = lexer_lex_all(l);
	// tokenToStringAll(l1);
	if (ac == 2 && !ft_strcmp("-i", av[1])) {
		char *input = NULL;
		while ((input = readline("> ")) != NULL) {
			Lexer_p l = lexer_init(input);
			TokenList *l1 = lexer_lex_all(l);
			tokenToStringAll(l1);
			add_history(input);
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
