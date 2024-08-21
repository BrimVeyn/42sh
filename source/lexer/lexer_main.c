/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:08:53 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/21 17:02:33 by bvan-pae         ###   ########.fr       */
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
			if (!tokenCmp(expected->filename, got->filename))
				return false;
			if (expected->fd_prefix != got->fd_prefix)
				return false;
			if (expected->fd_postfix != got->fd_postfix)
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
		case T_CONTROL_GROUP:
			if (expected->cs_type != got->cs_type)
				return false;
			if (expected->cs_list->size != got->cs_list->size)
				return false;
			for (uint16_t i = 0; i < expected->cs_list->size; i++) {
				if (!tokenCmp(expected->cs_list->t[i], got->cs_list->t[i]))
					return false;
			}
			return tokenCmp(expected->cs_postfix, got->cs_postfix);
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
	switch(t->tag) {
		case T_REDIRECTION:
			printf(C_LIGHT_YELLOW"T_REDIRECTION"C_RESET" {\n");
			printOffset(offset + 4);
			printf("Type: "C_MEDIUM_YELLOW"%s\n"C_RESET, tagName(t->r_type));
			if (t->fd_prefix != -1) {
				printOffset(offset + 4);
				printf("FdPrefix: "C_PEACH"|%d|\n"C_RESET, t->fd_prefix);
			}
			if (t->fd_postfix != -1) {
				printOffset(offset + 4);
				printf("FdPostfix: "C_PEACH"|%d|\n"C_RESET, t->fd_postfix);
			}
			tokenToString(t->filename, offset + 4);
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
			printf(C_LIGHT_CYAN"T_EXPRESSION"C_RESET" {\n");
			printOffset(offset + 4);
			printf("Type: "C_MEDIUM_CYAN"%s\n"C_RESET, tagName(t->ex_type));
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
		case T_CONTROL_GROUP:
			printf(C_LIGHT_CYAN"T_CONTROL_GROUP"C_RESET" {\n");
			printOffset(offset);
			printf("Type: "C_MEDIUM_CYAN"%s\n"C_RESET, tagName(t->cs_type));
			printOffset(offset);
			printf("CgList "C_BRONZE"{\n"C_RESET);
			for (uint16_t i = 0; i < t->cs_list->size; i++) {
				tokenToString(t->cs_list->t[i], offset + 4);
			}
			printOffset(offset);
			printf("CgPostfix:");
			tokenToString(t->cs_postfix, offset + 4);
			printOffset(offset);
			printf(C_BRONZE"}\n"C_RESET);
			break;
		default:
			printf("Format not handled\n");
	}
}


//Used to generate a token of type T_REDIRECTION
Token *genRedirTok(type_of_redirection type, int16_t preFd, int16_t postFd, Token *filename) {
	Token *tok = (Token *) gc_add(ft_calloc(1, sizeof(Token)));
	Token self = {
		.tag = T_REDIRECTION,
		.r_type = type,
		.fd_prefix = preFd,
		.fd_postfix = postFd,
		.filename = filename,
	};
	*tok = self;
	return tok;
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

Token *genCtrlSubTok(type_of_command_grouping type, TokenList *tl, Token *postfix) {
	Token *self = gc_add(ft_calloc(1, sizeof(Token)));
	Token tok = {
		.tag = T_CONTROL_GROUP,
		.cs_type = type,
		.cs_list = tl,
		.cs_postfix = postfix,
	};
	*self = tok;
	return self;
}

int main(void) {
	//Basic redirection test
	gc_init();

	Token *none_token __attribute__((unused)) = genNoneTok();
	Token *eof_token __attribute__((unused)) = genEOFTok();

	char *test_input_1 = (char *) gc_add(ft_strdup("$(10<10 >makefile echo salut $(cat makefile))"));
	TokenList *tl_1 = token_list_init();
	token_list_add(tl_1, genExprTok(EX_WORD, none_token, "dekodeko", none_token));
	token_list_add(tl_1, genExprTok(EX_WORD, none_token, "salut", none_token));
	Token *expected_1 = genCtrlSubTok(CG_CONTROL_SUBSTITUTION, tl_1, none_token);
	Token *got_1 = oneTokenTest(test_input_1);
	tokenAssert(expected_1, got_1, test_input_1, 12);

	gc_cleanup();
}
