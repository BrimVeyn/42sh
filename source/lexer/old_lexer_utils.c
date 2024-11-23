/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   old_lexer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 14:56:57 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/23 22:52:15 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/lexer.h"
#include "../../libftprintf/header/libft.h"

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
