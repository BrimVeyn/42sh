/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:45:20 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/02 09:17:06 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

Lexer_p lexer_init(char *input) {
	Lexer_p lexer = gc_add(ft_calloc(1, sizeof(Lexer)));

	Lexer self = {
		.input = input,
		.input_len = ft_strlen(input),
		.position = 0,
		.read_position = 0,
		.ch = 0,
	};

	*lexer = self;
	lexer_read_char(lexer);

	return lexer;
}

//Allocate a empty token_ptr
Token *token_empty_init(void) {
	Token *self = (Token *) ft_calloc(1, sizeof(Token));
	if (!self) exit(EXIT_FAILURE);
	self->tag = T_NONE;
	return self;
}

void token_word_init(Token *token) {
	token->w_infix = NULL;
	token->w_postfix = (Token *) gc_add(token_empty_init());
}

//initialise token redir struct
void token_redirection_init(Token *token) {
	token->r_postfix = (Token *) gc_add(token_empty_init());
}
