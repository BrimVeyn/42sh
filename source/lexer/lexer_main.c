/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:08:53 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/20 19:36:33 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "lexer_enum.h"
#include "lexer_struct.h"
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

Token *oneTokenTest(char *input) {
	Lexer_p l = lexer_init(input);

	Token_or_Error tok = lexer_get_next_token(l);
	assert(tok.tag != ERROR);
	Token *maybe_token = tok.token;

	lexer_deinit(l);
	return maybe_token;
}

bool tokenCmp(Token *expected, Token *got) {
	if (expected->tag != got->tag) return false;

	switch(expected->tag) {
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
		default:
			printf("Missing comparison here !\n");
			return false;
	}
	return true;
}

void tokenToString(Token *t) {
	switch(t->tag) {
		case T_REDIRECTION:
			printf("T_REDIRECTION { Type: %s, PreFD: %d, PostFD: %d \n", tagName(t->r_type), t->fd_prefix, t->fd_postfix);
			tokenToString(t->filename);
			printf("}\n");
			break;
		case T_NONE:
			printf("T_NONE {}\n");
			break;
		case T_EXPRESSION:
			printf("T_EXPRESSION { ");
			printf("Type: %s, ExPrefix: \n", tagName(t->ex_type));
			tokenToString(t->ex_prefix);
			printf("ex_infix: |%s| ExSuffix: \n", t->ex_infix);
			tokenToString(t->ex_postfix);
			printf("}\n");
			break;
		default:
			printf("Format not handled\n");
	}
}

//Custom assert to check whether two tokens are equal, prints debug if they're not
void tokenAssert(Token *expected, Token *got, char *input, int id) {
	if (!tokenCmp(expected, got)) {
		printf("Input: "C_YELLOW"%s\n"C_RESET, input);
		printf(C_GREEN"EXPECTED :\n"C_RESET);
		tokenToString(expected);
		printf(C_RED"GOT :\n"C_RESET);
		tokenToString(got);
	} else {
		printf(C_GREEN"Test %d passed !\n", id);
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

int main(void) {
	//Basic redirection test
	gc_init();

	Token *none_token = genNoneTok();

	char *test_input_1 = (char *) gc_add(ft_strdup("10>file"));
	Token *expr_tok_1 = genExprTok(EX_WORD, none_token, "file", none_token);
	Token *expected_1 = genRedirTok(R_OUTPUT, 10, -1, expr_tok_1);
	Token *got_1 = oneTokenTest(test_input_1);
	tokenAssert(expected_1, got_1, test_input_1, 1);

	char *test_input_2 = (char *) gc_add(ft_strdup("10>$(echo salut)"));
	Token *expr_tok_2 = genExprTok(EX_CONTROL_SUBSTITUTION, none_token, "echo salut", none_token);
	Token *expected_2 = genRedirTok(R_OUTPUT, 10, -1, expr_tok_2);
	Token *got_2 = oneTokenTest(test_input_2);
	tokenAssert(expected_2, got_2, test_input_2, 2);

	char *test_input_3 = (char *) gc_add(ft_strdup("10>$(echo $(echo salut))"));
	Token *expr_tok_postfix = genExprTok(EX_CONTROL_SUBSTITUTION, none_token, "echo salut", none_token);
	Token *expr_tok_3 = genExprTok(EX_CONTROL_SUBSTITUTION, none_token, "echo ", expr_tok_postfix);
	Token *expected_3 = genRedirTok(R_OUTPUT, 10, -1, expr_tok_3);
	Token *got_3 = oneTokenTest(test_input_3);
	tokenAssert(expected_3, got_3, test_input_3, 3);

	char *test_input_4 = (char *) gc_add(ft_strdup("10>$(echo slt$(echo $(echo fdp)))"));
	Token *expr_2 = genExprTok(EX_CONTROL_SUBSTITUTION, none_token, "echo fdp", none_token);
	Token *expr_3 = genExprTok(EX_CONTROL_SUBSTITUTION, none_token, "echo ", expr_2);
	Token *expr_tok_4 = genExprTok(EX_CONTROL_SUBSTITUTION, none_token, "echo slt", expr_3);
	Token *expected_4 = genRedirTok(R_OUTPUT, 10, -1, expr_tok_4);
	Token *got_4 = oneTokenTest(test_input_4);
	tokenAssert(expected_4, got_4, test_input_4, 4);

	char *test_input_5 = (char *) gc_add(ft_strdup("10>$(echo salut)homme"));
	Token *expr_5 = genExprTok(EX_WORD, none_token, "homme", none_token);
	Token *expr_tok_5 = genExprTok(EX_CONTROL_SUBSTITUTION, none_token, "echo salut", expr_5);
	Token *expected_5 = genRedirTok(R_OUTPUT, 10, -1, expr_tok_5);
	Token *got_5 = oneTokenTest(test_input_5);
	tokenAssert(expected_5, got_5, test_input_5, 5);

	char *test_input_6 = (char *) gc_add(ft_strdup("10>$(echo salut)homme$(echo salut)"));
	Token *expr_7 = genExprTok(EX_CONTROL_SUBSTITUTION, none_token, "echo salut", none_token);
	Token *expr_6 = genExprTok(EX_WORD, none_token, "homme", expr_7);
	Token *expr_tok_6 = genExprTok(EX_CONTROL_SUBSTITUTION, none_token, "echo salut", expr_6);
	Token *expected_6 = genRedirTok(R_OUTPUT, 10, -1, expr_tok_6);
	Token *got_6 = oneTokenTest(test_input_6);
	tokenAssert(expected_6, got_6, test_input_6, 6);

	char *test_input_7 = (char *) gc_add(ft_strdup("10>file"));
	Token *expr_tok_7 = genExprTok(EX_WORD, none_token, "file", none_token);
	Token *expected_7 = genRedirTok(R_OUTPUT, 10, -1, expr_tok_7);
	Token *got_7 = oneTokenTest(test_input_7);
	tokenAssert(expected_7, got_7, test_input_7, 7);

	char *test_input_8 = (char *) gc_add(ft_strdup("10>homme$(echo femme)"));
	Token *expr_9 = genExprTok(EX_CONTROL_SUBSTITUTION, none_token, "echo femme", none_token);
	Token *expr_8 = genExprTok(EX_WORD, none_token, "homme", expr_9);
	Token *expected_8 = genRedirTok(R_OUTPUT, 10, -1, expr_8);
	Token *got_8 = oneTokenTest(test_input_8);
	tokenAssert(expected_8, got_8, test_input_8, 8);

	char *test_input_9 = (char *) gc_add(ft_strdup("10>hom$(echo me)mer$(echo hey)no"));
	Token *expr_13 = genExprTok(EX_WORD, none_token, "no", none_token);
	Token *expr_12 = genExprTok(EX_CONTROL_SUBSTITUTION, none_token, "echo hey", expr_13);
	Token *expr_11 = genExprTok(EX_WORD, none_token, "mer", expr_12);
	Token *expr_10 = genExprTok(EX_CONTROL_SUBSTITUTION, none_token, "echo me", expr_11);
	Token *expr_tok_9 = genExprTok(EX_WORD, none_token, "hom", expr_10);
	Token *expected_9 = genRedirTok(R_OUTPUT, 10, -1, expr_tok_9);
	Token *got_9 = oneTokenTest(test_input_9);
	tokenAssert(expected_9, got_9, test_input_9, 9);
	
	// char *test_input_3 = (char *) gc_add(ft_strdup("10>>file"));
	// Token *expected_3 = redirectionGen(R_APPEND, 10, -1, "file");
	// Token got_3 = oneTokenTest(test_input_3);
	// tokenAssert(*expected_3, got_3, test_input_3);
	//
	// char *test_input_4 = (char *) gc_add(ft_strdup("10<<file"));
	// Token *expected_4 = redirectionGen(R_HERE_DOC, 10, -1, "file");
	// Token got_4 = oneTokenTest(test_input_4);
	// tokenAssert(*expected_4, got_4, test_input_4);
	//
	// char *test_input_5 = (char *) gc_add(ft_strdup("10<&file"));
	// Token *expected_5 = redirectionGen(R_DUP_IN, 10, -1, "file");
	// Token got_5 = oneTokenTest(test_input_5);
	// tokenAssert(*expected_5, got_5, test_input_5);
	//
	// char *test_input_6 = (char *) gc_add(ft_strdup("10>&file"));
	// Token *expected_6 = redirectionGen(R_DUP_OUT, 10, -1, "file");
	// Token got_6 = oneTokenTest(test_input_6);
	// tokenAssert(*expected_6, got_6, test_input_6);
	//
	// char *test_input_7 = (char *) gc_add(ft_strdup("&>file"));
	// Token *expected_7 = redirectionGen(R_DUP_BOTH, -1, -1, "file");
	// Token got_7 = oneTokenTest(test_input_7);
	// tokenAssert(*expected_7, got_7, test_input_7);
	//
	// char *test_input_8 = (char *) gc_add(ft_strdup("&>>file"));
	// Token *expected_8 = redirectionGen(R_DUP_BOTH_APPEND, -1, -1, "file");
	// Token got_8 = oneTokenTest(test_input_8);
	// tokenAssert(*expected_8, got_8, test_input_8);

	gc_cleanup();
}
