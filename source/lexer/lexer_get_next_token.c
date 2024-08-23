/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_get_next_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 15:53:46 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/23 16:31:20 by bvan-pae         ###   ########.fr       */
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
static void eat_whitespace(Lexer_p l) {
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

void read_till_next_scalar(Lexer_p l, char c) {
	while (l->ch != c) {
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
		printf("eof\n");
		return T_END_OF_FILE;
	}
	if (ft_strchr("><", l->ch)
		|| !ft_strncmp("&>", &l->input[l->position], 2)
		|| !ft_strncmp("&>>", &l->input[l->position], 3)) {
		return T_REDIRECTION;
	}
	if (!ft_strncmp("$(", &l->input[l->position], 2)
		|| !ft_strncmp("{ ", &l->input[l->position], 2)
		|| l->ch == '(') {
		return T_COMMAND_GROUPING;
	}
	if (!ft_strncmp("&&", &l->input[l->position], 2)
		|| !ft_strncmp("||", &l->input[l->position], 2)
		||	l->ch == '&' || l->ch == '|' || l->ch == ';') {
		return T_SEPARATOR;
	}
	return T_EXPRESSION;
}

void init_token_from_tag(Token *token) {
	switch (token->tag) {
		case T_REDIRECTION:
			token_redirection_init(token);
			break;
		case T_COMMAND_GROUPING:
			token_command_grouping_init(token);
			break;
		case T_EXPRESSION:
			token_expression_init(token);
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

type_of_command_grouping get_command_grouping_type(Lexer_p l) {
	if (!ft_strncmp("$(", &l->input[l->position], 2)) {
		lexer_read_x_char(l, 2);
		return CG_CONTROL_SUBSTITUTION;
	}
	else if (!ft_strncmp("{ ", &l->input[l->position], 2)) {
		lexer_read_x_char(l, 2);
		return CG_CONTROL_GROUP;
	}
	else {
		lexer_read_char(l);
		return CG_SUBSHELL;
	}
}

type_of_expression get_expression_type(Lexer_p l) {
	if (!ft_strncmp("${", &l->input[l->position], 2)) {
		lexer_read_x_char(l, 2);
		return EX_VARIABLE_EXPANSION;
	}
	else if (ft_strchr("*[]?!", l->ch)) {
		lexer_read_char(l);
		return EX_PATTERN_MATCHING;
	}
	else if (l->ch == '\"') {
		lexer_read_char(l);
		return EX_INHIBITOR_DQUOTE;
	}
	else if (l->ch == '\'') {
		lexer_read_char(l);
		return EX_INHIBITOR_SQUOTE;
	}
	else if (l->ch == '=') {
		lexer_read_char(l);
		return EX_VARIABLE_ASSIGNMENT;
	}
	else if (l->ch == '{'){
		lexer_read_char(l);
		return EX_BRACES_EXPANSION;
	}
	else {
		return EX_WORD;
	}
}

char *get_till_sq(Token *token, Lexer_p l) {
	const uint16_t start = l->position;
	while (l->ch && l->ch != '\'') {
		if (!ft_strncmp("\\\'", &l->input[l->position], 2)) {
			lexer_read_x_char(l, 2);
			token->e = ERROR_ESCAPED_SQ;
		}
		lexer_read_char(l);
    }
	const uint16_t end = l->position;

	if (l->ch != '\'')
		token->e = ERROR_UNCLOSED_SQ;
	else
		lexer_read_char(l);

	return (char *) gc_add(ft_substr(l->input, start, end - start));
}

char *get_till_dq(Token *token, Lexer_p l) {
	const uint16_t start = l->position;
	while (l->ch && l->ch != '\"') {
		lexer_read_char(l);
    }
	const uint16_t end = l->position;

	if (l->ch != '\"')
		token->e = ERROR_UNCLOSED_DQ;
	else
		lexer_read_char(l);

	return (char *) gc_add(ft_substr(l->input, start, end - start));
}

bool is_special_char(char c) {
	return ft_strchr("<>|&;[]?!*=\'\"", c);
}

bool is_special_slice(Lexer_p l) {
	if (!ft_strncmp("${", &l->input[l->position], 2) ||
		!ft_strncmp("$(", &l->input[l->position], 2))
	{
		return true;
	}
	return false;
}

bool is_special(Lexer_p l) {
	return (is_special_slice(l) || is_special_char(l->ch));
}

char *get_till_special_or_whitespace(Lexer_p l) {
	const uint16_t start = l->position;
	// if (is_special(l)) lexer_read_char(l);
	while (l->ch && !is_special(l) && !is_whitespace(l->ch)) {
		lexer_read_char(l);
	}
	const uint16_t end = l->position;
	return (char *) gc_add(ft_substr(l->input, start, end - start));
}

type_of_error check_identifier_syntax(char *identifier) {
	if (ft_strlen(identifier) == 1 && *identifier == '?') return ERROR_NONE;
	if (!ft_isalpha(*identifier) && !(*identifier == '_')) {
		return ERROR_BAD_SUBSTITUTION;
	} else identifier += 1;
	while (*identifier != '\0') {
		if (!ft_isalnum(*identifier) && !(*identifier == '_')) {
			return ERROR_BAD_SUBSTITUTION;
		}
		identifier += 1;
	}
	return ERROR_NONE;
}

char *get_identifier(Token *token, Lexer_p l) {
	const uint16_t start = l->position;
	const char target = '}';
	uint16_t found = 0;
	uint16_t needed = 1;
	while (l->ch) {
		if (l->ch == '{') needed += 1;
		if (l->ch == target) found += 1;
		if (needed == found) break;
		lexer_read_char(l);
	}
	const uint16_t end = l->position;
	if (l->ch != target || needed != found || start == end) {
		token->e = ERROR_BAD_SUBSTITUTION;
	} else lexer_read_char(l);
	char *identifier = (char *) gc_add(ft_substr(l->input, start, end - start)); 
	token->e = check_identifier_syntax(identifier);
	return identifier;
}

char *get_till_end_of_cg(Token *token, Lexer_p l) {
	const uint16_t start = l->position;
	const char target = (token->cg_type == CG_SUBSHELL) * ')'
		+ (token->cg_type == CG_CONTROL_SUBSTITUTION) * ')'
		+ (token->cg_type == CG_CONTROL_GROUP) * '}';
	uint16_t found = 0;
	uint16_t needed = 1;
	while (l->ch) {
		if (token->cg_type == CG_CONTROL_SUBSTITUTION && l->ch == '(') {
			lexer_read_char(l);
			needed += 1;
		} else if (token->cg_type == CG_CONTROL_GROUP && l->ch == '{') {
			lexer_read_char(l);
			needed += 1;
		}
		if (l->ch == target) found += 1;
		if (needed == found) break;
		lexer_read_char(l);
	}
	const uint16_t end = l->position;
	// const char *
	if (l->ch != target || needed != found) {
		token->e = ERROR_UNCLOSED_CG;
	} else lexer_read_char(l);
	return (char *) gc_add(ft_substr(l->input, start, end - start));
}
bool is_valid_control_group(TokenList *list) {
	if (list->size < 2) return false;
	return true;
}

//Extract next token from the input
Token *lexer_get_next_token(Lexer_p l, bool recursive_call) {
	Token *token = gc_add(ft_calloc(1, sizeof(Token)));

	eat_whitespace(l);

	token->tag = get_token_tag(l);
	//Here we unchain all separators if they were following any expression w/o whitespaces
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
			token->r_postfix = lexer_get_next_token(l, true);
			break;
		case T_COMMAND_GROUPING:
			token->cg_type = get_command_grouping_type(l);
			token->cg_lexer = lexer_init(get_till_end_of_cg(token, l));
			token->cg_list = lexer_lex_all(token->cg_lexer);
			if (!is_valid_control_group(token->cg_list)) token->e = ERROR_UNEXPCTED_TOKEN;
			if (!is_whitespace(l->ch)) {
				token->cg_postfix = lexer_get_next_token(l, true);
			}
			break;
		case T_EXPRESSION:
			token->ex_type = get_expression_type(l);
			switch (token->ex_type) {
				case EX_INHIBITOR_SQUOTE:
					token->ex_infix = get_till_sq(token, l); break;
				case EX_INHIBITOR_DQUOTE:
					token->ex_infix = get_till_dq(token, l); break;
				case EX_VARIABLE_EXPANSION:
					token->ex_infix = get_identifier(token, l); break;
				case EX_WORD:
					token->ex_infix = get_till_special_or_whitespace(l); break;
				default:
					token->ex_infix = NULL;
			}
			if (!is_whitespace(l->ch)) {
				token->ex_postfix = lexer_get_next_token(l, true);
			}
			break;
		default:
			lexer_read_char(l);
			break;
	}
	return token;
}
