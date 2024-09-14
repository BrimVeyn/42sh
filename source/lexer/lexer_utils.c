/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 10:19:29 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/14 16:34:51 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"
#include "lexer.h"

void lexer_read_char(Lexer_p l) {
	if (l->read_position >= l->input_len) {
		l->ch = 0;
	} else {
		l->ch = l->input[l->read_position];
	}
	l->position = l->read_position;
	l->read_position += 1;
}

void lexer_read_x_char(Lexer_p l, uint16_t n) {
	for (uint16_t i = 0; i < n; i++) {
		lexer_read_char(l);
	}
}

void skip_command_sub(Lexer_p l) {
	lexer_read_char(l);
	while (l->ch && l->ch != ')') {
		if (l->ch == '(') {
			skip_command_sub(l);
		}
		lexer_read_char(l);
	}
}

char *get_word(Lexer_p l, type_mode mode) {
	const uint16_t start = l->position;
	while (l->ch != '\0') {
		if (!ft_strncmp(&l->input[l->position], "$(", 2)) {
			lexer_read_char(l);
			skip_command_sub(l);
		}
		lexer_read_char(l);
		if (is_delimiter(mode, l->ch)) {
			break;
		}
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

bool is_redirection_char(char c) {
	return (c == '>' || c == '<');
}

bool is_number(char *str) {
	for (uint16_t i = 0; str[i]; i++) {
		if (!ft_isdigit(str[i])) return false;
	}
	return true;
}

bool is_delimiter(type_mode mode, char c) {
	if (mode == DEFAULT) {
		return ft_strchr("|&<>();\n \t", c) || c == '\0';
	} else {
		return ft_strchr("$", c) || c == '\0';
	}
}

void free_charchar(char **s){
	for (int i = 0; s[i]; i++){
		free(s[i]);
	}
	free(s);
}
