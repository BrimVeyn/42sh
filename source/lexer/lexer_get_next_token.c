/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_get_next_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 15:53:46 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/27 10:32:41 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "lexer_enum.h"
#include "lexer_struct.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//Read a char from input
void lexer_read_char(Lexer_p l) {
	if (l->read_position >= l->input_len) {
		l->ch = 0;
	} else {
		l->ch = l->input[l->read_position];
	}
	l->position = l->read_position;
	l->read_position += 1;
}

//Skips all whitespaces
void eat_whitespace(Lexer_p l) {
	while (is_whitespace(l->ch)) {
		lexer_read_char(l);
	}
}

//assert that the next word is a number
void eat_scalar(Lexer_p l, char c) {
	while (l->ch == c) {
		lexer_read_char(l);
	}
}

void lexer_read_x_char(Lexer_p l, uint16_t n) {
	for (uint16_t i = 0; i < n; i++) {
		lexer_read_char(l);
	}
}

type_of_token get_token_tag(Lexer_p l) {
	if (ft_strchr("&|;\0\n", l->ch)) {
		return T_SEPARATOR;
	}
	if (ft_strchr("><", l->ch)
		|| !ft_strncmp("&>", &l->input[l->position], 2)
		|| !ft_strncmp("&>>", &l->input[l->position], 3)) {
		return T_REDIRECTION;
	}
	if (!ft_strncmp("$(", &l->input[l->position], 2)
		|| l->ch == '(') {
		return T_GROUPING;
	}
	return T_NONE;
}

void init_token_from_tag(Token *token) {
	switch (token->tag) {
		case T_REDIRECTION:
			token_redirection_init(token);
			break;
		case T_GROUPING:
			token_command_grouping_init(token);
			break;
		default:
			return;
	}
}

type_of_separator get_separator_type(Lexer_p l) {
	if (!ft_strncmp("&&", &l->input[l->position], 2)) {
		lexer_read_x_char(l, 2);
		return S_AND;
	}
	else if (!ft_strncmp("||", &l->input[l->position], 2)) {
		lexer_read_x_char(l, 2);
		return S_OR;
	}
	else if (l->ch == '|') {
		lexer_read_char(l);
		return S_PIPE;
	}
	else if (l->ch == ';') {
		lexer_read_char(l);
		return S_SEMI;
	}
	else {
		lexer_read_char(l);
		return S_BG;
	}
}

type_of_redirection get_redirection_type(Lexer_p l) {
	if (!ft_strncmp("&>>", &l->input[l->position], 3)) {
		lexer_read_x_char(l, 3);
		return R_DUP_BOTH_APPEND;
	}
	else if (!ft_strncmp(">>", &l->input[l->position], 2)) {
		lexer_read_x_char(l, 2);
		return R_APPEND;
	}
	else if (!ft_strncmp("<<", &l->input[l->position], 2)) {
		lexer_read_x_char(l, 2);
		return R_HERE_DOC;
	}
	else if (!ft_strncmp("<&", &l->input[l->position], 2)) {
		lexer_read_x_char(l, 2);
		return R_DUP_IN;
	}
	else if (!ft_strncmp(">&", &l->input[l->position], 2)) {
		lexer_read_x_char(l, 2);
		return R_DUP_OUT;
	}
	else if (!ft_strncmp("&>", &l->input[l->position], 2)) {
		lexer_read_x_char(l, 2);
		return R_DUP_BOTH;
	}
	else if (l->ch == '>') {
		lexer_read_char(l);
		return R_OUTPUT;
	}
	else {
		lexer_read_char(l);
		return R_INPUT;
	}
}

//Extract next token from the input
Token *lexer_get_next_token(Lexer_p l, bool recursive_call, const char *delimiters) {
	Token *token = gc_add(ft_calloc(1, sizeof(Token)));

	eat_whitespace(l);

	token->tag = get_token_tag(l); //Here we unchain all separators if they were following any expression w/o whitespaces
	if (recursive_call && token->tag == T_SEPARATOR) {
		token->tag = T_NONE;
		return token;
	}
	token->e = ERROR_NONE;
	init_token_from_tag(token);

	switch(token->tag) {
		case T_SEPARATOR:
			token->s_type = get_separator_type(l); break;
		case T_REDIRECTION:
			token->r_type = get_redirection_type(l);
			eat_whitespace(l);
			break;
		case T_GROUPING:
			token->delimiters = DELIMITERS_DQUOTE;
			//oui
			break;
		default:
			printf("Default case of get_next_token !\n");
	}
	return token;
}
