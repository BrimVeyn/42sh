/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_get_next_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 15:53:46 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/30 13:53:34 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "lexer_enum.h"
#include "lexer_struct.h"
#include <stdint.h>

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
	if (l->ch == '\0') {
		return T_SEPARATOR;
	}
	if (ft_strchr("><", l->ch)
		|| !ft_strncmp("&>", &l->input[l->position], 2)
		|| !ft_strncmp("&>>", &l->input[l->position], 3)) {
		return T_REDIRECTION;
	}
	if (ft_strchr("&|;\n)", l->ch)) {
		return T_SEPARATOR;
	}
	if (!ft_strncmp("$(", &l->input[l->position], 2)
		|| ft_strchr("(\"", l->ch)) {
		return T_GROUPING;
	}
	return T_WORD;
}

void init_token_from_tag(Token *token) {
	switch (token->tag) {
		case T_REDIRECTION:
			token_redirection_init(token);
			break;
		case T_GROUPING:
			token_command_grouping_init(token);
			break;
		case T_WORD:
			token_word_init(token);
		default:
			return;
	}
}

type_of_separator get_separator_type(Lexer_p l) {
	if (l->ch == '\0') {
		lexer_read_char(l);
		return S_EOF;
	}
	else if (l->ch == '\n') {
		lexer_read_char(l);
		return S_NEWLINE;
	}
	else if (!ft_strncmp("&&", &l->input[l->position], 2)) {
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
		return S_SEMI_COLUMN;
	}
	else if (l->ch == ')') {
		lexer_read_char(l);
		return S_PAR_CLOSE;
	}
	else if (l->ch == '&'){
		lexer_read_char(l);
		return S_BG;
	} else {
		printf("error separator type l->ch = %d %c\n", l->ch, l->ch);
		exit(EXIT_FAILURE);
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
	else if (l->ch == '<'){
		lexer_read_char(l);
		return R_INPUT;
	} else {
		printf("Error lexing redirection !\n");
		exit(EXIT_FAILURE);
	}
}

type_of_grouping get_group_type(Lexer_p l) {
	if (!ft_strncmp("$(", &l->input[l->position], 2)) {
		lexer_read_x_char(l, 2);
		return G_COMMAND_SUB;
	} else if (l->ch == '(') { 
		lexer_read_char(l);
		return G_SUBSHELL;
	} else if (l->ch == '\"'){
		lexer_read_char(l);
		return G_DQUOTE;
	} else {
		printf("char = %c %d\n", l->ch, l->ch);
		exit(EXIT_FAILURE);
	}
}

TokenList *lexer_lex_till(Lexer_p l, type_of_separator sep) {
	TokenList *self = token_list_init();
	while (true) {
		Token *tmp = lexer_get_next_token(l, false);
		token_list_add(self, tmp);
		if (tmp->tag == T_SEPARATOR && (tmp->s_type == sep || tmp->s_type == S_EOF))
			break;
	}
	return self;
}



bool is_delimiter(type_mode mode, char c) {
	if (mode == DEFAULT) {
		return ft_strchr("|&<>$();\n \t", c) || c == '\0';
	} else {
		return ft_strchr("$\n", c) || c == '\0';
	}
}

char *get_word(Lexer_p l) {
	const uint16_t start = l->position;
	while (l->ch != '\0') {
		lexer_read_char(l);
		if (is_delimiter(l->mode, l->ch)) {
			break;
		}
	}
	const uint16_t end = l->position;
	return (char *) gc_add(ft_substr(l->input, start, end - start));
}


//Extract next token from the input
Token *lexer_get_next_token(Lexer_p l, bool recursive_call) {
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
			token->s_type = get_separator_type(l); 
			break;
		case T_REDIRECTION:
			token->r_type = get_redirection_type(l);
			eat_whitespace(l);
			token->r_postfix = lexer_get_next_token(l, true);
			if (token->r_postfix->tag == T_NONE) {
				token->e = ERROR_UNEXPECTED_TOKEN;
			}
			break;
		case T_GROUPING:
			token->g_type = get_group_type(l);
			if (token->g_type == G_DQUOTE) {
				token->g_list = lexer_lex_till(l, S_DQ);
			} else {
				token->g_list = lexer_lex_till(l, S_PAR_CLOSE);
			}
			if (!is_whitespace(l->ch)) {
				token->g_postfix = lexer_get_next_token(l, true);
			}
			break;
		case T_WORD:
			token->w_infix = get_word(l);
			if (!is_whitespace(l->ch) && is_fdable_redirection(l)) {
				token->w_postfix = lexer_get_next_token(l, true);
			} else {
				break;
			}
			if (token->w_postfix->tag == T_REDIRECTION && 
				token->w_postfix->r_postfix->tag == T_NONE) {
				token->e = ERROR_UNEXPECTED_TOKEN;
			}
			break;
		default:
			printf("Default case of get_next_token !\n");
	}
	return token;
}
