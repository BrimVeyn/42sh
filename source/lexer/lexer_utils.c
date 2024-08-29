/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 09:13:03 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/29 11:49:43 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

//Custom implementation of realloc
void *ft_realloc(void *ptr, size_t oldSize, size_t nbEl, size_t elSize) {
	void *new_ptr = malloc(nbEl * elSize);
	if (!new_ptr)
		exit(EXIT_FAILURE);
	ft_memcpy(new_ptr, ptr, oldSize * elSize);
	free(ptr);
	return new_ptr;
}

//Return true if char is a whitespace
bool is_whitespace(char c) {
	return (c == ' ' || c == '\t');
}

//Returns true if char is < or >
bool is_redirection_char(char c) {
	return (c == '>' || c == '<');
}

bool is_number(char *str) {
	for (uint16_t i = 0; str[i]; i++) {
		if (!ft_isdigit(str[i])) return false;
	}
	return true;
}

bool next_token_is_redirection(Lexer_p l) {
	const char *input_ptr = &l->input[l->position];
	if (!ft_strncmp(input_ptr, "&>", 2) ||
		!ft_strncmp(input_ptr, ">&", 2) ||
		!ft_strncmp(input_ptr, "<&", 2) ||
		!ft_strncmp(input_ptr, ">>", 2) ||
		!ft_strncmp(input_ptr, "<<", 2) ||
		ft_strchr("<>", l->ch)) {
		return true;
	}
	return false;
}
