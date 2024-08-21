/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:45:20 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/21 16:33:41 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "lexer_enum.h"

Lexer_p lexer_init(char *input) {
	Lexer_p lexer = gc_add(ft_calloc(1, sizeof(Lexer)));

	Lexer self = {
		.input = input,
		.input_len = ft_strlen(input),
		.position = 0,
		.read_position = 0,
		.ch = 0,
		.peak_position = 0,
		.peak_ch = 0,
	};

	*lexer = self;
	lexer_read_char(lexer);

	return lexer;
}

//Allocate a empty token_ptr
Token *token_empty_init(void) {
	Token *self = ft_calloc(1, sizeof(Token));
	self->tag = T_NONE;
	return self;
}

void token_control_group_init(Token *token) {
	token->tag = T_CONTROL_GROUP;
	token->cs_postfix = gc_add(token_empty_init());
}

//initialise expr token
void token_expression_init(Token *token) {
	token->tag = T_EXPRESSION;
	token->ex_prefix = gc_add(token_empty_init());
	token->ex_infix = NULL;
	token->ex_postfix = gc_add(token_empty_init());
}

//initialise token redir struct
void token_redirection_init(Token *token) {
	token->tag = T_REDIRECTION;
	token->fd_prefix = -1;
	token->fd_postfix = -1;
	token->filename = gc_add(token_empty_init());
}
