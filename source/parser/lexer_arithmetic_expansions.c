/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_arithmetic_expansions.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 11:46:55 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/20 16:01:21 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

arithemtic_token_tag get_atoken_tag(Lexer_p l) {
	if (regex_match("^[0-9a-zA-Z]", &l->input[l->position]).start != -1) {
		return A_OPERAND;
	} else {
		return A_OPERATOR;
	}
}

AToken *lexer_get_next_atoken(Lexer_p l) {
	AToken *self = gc_add(ft_calloc(1, sizeof(AToken *)), GC_SUBSHELL);

	eat_whitespace(l);

	self->tag = get_atoken_tag(l);

	switch (self->tag) {
		case A_OPERATOR:
			lexer_read_char(l);
			break;
		case A_OPERAND:
			lexer_read_char(l);
			break;
		default:
			dprintf(2, "Fatal arithmetic expansion lexer\n");
			exit(EXIT_FAILURE);
	}
	return self;
}

ATokenList *lexer_arithmetic_exp_lex_all(Lexer_p lexer) {
	ATokenList *self = atoken_list_init();

	while (lexer->ch) {
		AToken *tmp = lexer_get_next_atoken(lexer);
		atoken_list_add(self, tmp);
	}

	return self;
}
