/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 09:13:03 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/27 16:31:23 by bvan-pae         ###   ########.fr       */
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

//Returns true if char is 0..9
bool is_number(char c) {
	return (c >= '0' && c <= '9');
}
