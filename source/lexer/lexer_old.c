/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_old.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 10:43:33 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/24 09:36:24 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "utils.h"
#include "libft.h"

TokenList *lexer_lex_all(char *input) {
	Lexer_p l = lexer_init(input);
	da_create(self, TokenList, sizeof(Token *), GC_SUBSHELL);
	while (true) {
		Token *tmp = lexer_get_next_token(l);
		da_push(self, tmp);
		if (tmp->tag == T_SEPARATOR && tmp->s_type == S_EOF) break;
	}
	return self;
}

void init_token_from_tag(Token *token) {
	switch (token->tag) {
		case T_REDIRECTION:
			token_redirection_init(token);
			break;
		case T_WORD:
			token_word_init(token);
		default:
			return;
	}
}

//Extract next token from the input
Token *lexer_get_next_token(Lexer_p l) {
	Token *token = gc(GC_ADD, ft_calloc(1, sizeof(Token)), GC_SUBSHELL);

	eat_whitespace(l);

	token->tag = get_token_tag(l);
	token->e = ERROR_NONE;

	init_token_from_tag(token);

	switch(token->tag) {
		case T_SEPARATOR:
			token->s_type = get_separator_type(l); 
			break;
		case T_REDIRECTION:
			token->r_type = get_redirection_type(l);
			eat_whitespace(l);
			token->r_postfix = lexer_get_next_token(l);
			if (token->r_postfix->tag != T_WORD) {
				token->e = ERROR_UNEXPECTED_TOKEN;
			}
			break;
		case T_WORD:
			token->w_infix = get_word(l);
			if (is_number(token->w_infix) && !is_whitespace(l->ch) && is_fdable_redirection(l)) {
				token->w_postfix = lexer_get_next_token(l);
			} else break;
			if (token->w_postfix->tag == T_REDIRECTION && 
				token->w_postfix->r_postfix->tag == T_NONE) {
				token->e = ERROR_UNEXPECTED_TOKEN;
			}
			break;
		default:
			printf("Default case of get_next_token !\n");
			exit(EXIT_FAILURE);
			break;
	}
	return token;
}
