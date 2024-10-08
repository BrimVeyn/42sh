/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:45:20 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/02 12:47:37 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "utils.h"
#include "libft.h"

#include <stdlib.h>

Lexer_p lexer_init(char *input) {
	Lexer_p lexer = gc(GC_ADD, ft_calloc(1, sizeof(Lexer)), GC_SUBSHELL);

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
	if (!self)
		exit(EXIT_FAILURE);
	self->tag = T_NONE;
	return self;
}

void token_word_init(Token *token) {
	token->w_infix = NULL;
	token->w_postfix = (Token *) gc(GC_ADD, token_empty_init(), GC_SUBSHELL);
}

//initialise token redir struct
void token_redirection_init(Token *token) {
	token->r_postfix = (Token *) gc(GC_ADD, token_empty_init(), GC_SUBSHELL);
}
