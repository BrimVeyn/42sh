/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:20:37 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/26 15:07:40 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/lexer.h"
#include "../../include/utils.h"
#include "../../libftprintf/header/libft.h"

#include <stdio.h>
#include <stdlib.h>

void lexer_read_char(Lexer_p l) {
	if (l->read_position >= l->input_len) {
		l->ch = 0;
	} else {
		l->ch = l->input[l->read_position];
	}
	l->position = l->read_position;
	l->read_position += 1;
}

void lexer_reverse_read_char(Lexer_p l) {
	l->read_position -= 1;
	l->position = l->read_position;
	l->ch = l->input[l->read_position];
}

void lexer_read_x_char(Lexer_p l, uint16_t n) {
	for (uint16_t i = 0; i < n; i++) {
		lexer_read_char(l);
	}
}

static void skip_command_sub(Lexer_p l, int x) {
	lexer_read_x_char(l, x);
	while (l->ch && l->ch != ')') {
		if (!ft_strncmp(&l->input[l->position], "$(", 2)) {
			skip_command_sub(l, 2);
		}
		if (l->ch == '(') {
			skip_command_sub(l, 1);
		}
		lexer_read_char(l);
	}
}

static void skip_parameter_exp(Lexer_p l) {
	lexer_read_x_char(l, 2);
	while (l->ch && l->ch != '}') {
		if (!ft_strncmp(&l->input[l->position], "${", 2)) {
			skip_parameter_exp(l);
		}
		lexer_read_char(l);
	}
}

static void skip_parenthesis(Lexer_p l) {
	lexer_read_char(l);
	while (l->ch && l->ch != ')') {
		if (l->ch == '(') {
			skip_parenthesis(l);
		}
		lexer_read_char(l);
	}
}

static void skip_arithmetic_exp(Lexer_p l) {
	lexer_read_x_char(l, 3);
	while (l->ch) {
		if (l->ch == '(') {
			skip_parenthesis(l);
			lexer_read_char(l);
		}
		if (!ft_strncmp(&l->input[l->position], "$((", 3)) {
			skip_arithmetic_exp(l);
		}
		if (!ft_strncmp(&l->input[l->position], "))", 2)) {
			lexer_read_x_char(l, 2);
			return ;
		}
		lexer_read_char(l);
	}
}

char *get_word(Lexer_p l) {
	const uint16_t start = l->position;
	while (l->ch != '\0') {
		if (!ft_strncmp(&l->input[l->position], "$((", 3)) {
			skip_arithmetic_exp(l);
			lexer_reverse_read_char(l);
        }
		if (!ft_strncmp(&l->input[l->position], "$(", 2))
			skip_command_sub(l, 2);
		if (!ft_strncmp(&l->input[l->position], "${", 2))
			skip_parameter_exp(l);

		lexer_read_char(l);

		if (is_delimiter(l->ch))
			break;
	}
	const uint16_t end = l->position;
	return (char *) gc_add(ft_substr(l->input, start, end - start), GC_SUBSHELL);
}

void eat_whitespace(Lexer_p l) {
	while (is_whitespace(l->ch)) {
		lexer_read_char(l);
	}
}

bool is_fdable_redirection(Lexer_p l) {
	if (l->ch == '\0') return false;
	const char *input_ptr = &l->input[l->position];
	const size_t input_len = ft_strlen(input_ptr);

	if ((!ft_strncmp(input_ptr, ">&", 2) || !ft_strncmp(input_ptr, "<&", 2)) ||
		(input_len >= 1 && ft_strchr("<>", l->ch))) {
		return true;
	}
	return false;
}

bool is_whitespace(char c) {
	return (c == ' ' || c == '\t');
}

bool is_number(char *str) {
	for (uint16_t i = 0; str[i]; i++) {
		if (!ft_isdigit(str[i])) return false;
	}
	return true;
}

bool is_delimiter(const char c) {
	return ft_strchr("|&<>();\n \t", c) || c == '\0';
}

