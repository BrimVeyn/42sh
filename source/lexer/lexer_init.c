/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:45:20 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/22 16:39:03 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "lexer_enum.h"
#include "lexer_struct.h"

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
	Token *self = ft_calloc(1, sizeof(Token));
	self->tag = T_NONE;
	return self;
}

void token_command_grouping_init(Token *token) {
	token->cg_list = NULL;
	token->cg_postfix = (Token *) gc_add(token_empty_init());
}

//initialise expr token
void token_expression_init(Token *token) {
	token->ex_prefix = (Token *) gc_add(token_empty_init());
	token->ex_infix = NULL;
	token->ex_postfix = (Token *) gc_add(token_empty_init());
}

//initialise token redir struct
void token_redirection_init(Token *token) {
	token->r_postfix = (Token *) gc_add(token_empty_init());
}
