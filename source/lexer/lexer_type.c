/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_type.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 13:01:17 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/04 12:05:10 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "libft.h"

#include <stdlib.h>
#include <stdio.h>

type_of_token get_token_tag(Lexer_p l) {
	if (l->ch == '\0') {
		return T_SEPARATOR;
	}
	if (ft_strchr("><", l->ch)
		|| !ft_strncmp("&>", &l->input[l->position], 2)
		|| !ft_strncmp("&>>", &l->input[l->position], 3)) {
		return T_REDIRECTION;
	}
	if (ft_strchr("&|;()\"\n)", l->ch)) {
		return T_SEPARATOR;
	}
	return T_WORD;
}

type_of_separator get_separator_type(Lexer_p l) {
	switch (l->ch) {
		case '\0': lexer_read_char(l); return S_EOF;
		case '\n': lexer_read_char(l); return S_NEWLINE;
	}

	if (!ft_strncmp("&&", &l->input[l->position], 2)) {
		lexer_read_x_char(l, 2);
		return S_AND;
	} else if (!ft_strncmp("||", &l->input[l->position], 2)) {
		lexer_read_x_char(l, 2);
		return S_OR;
	}

	switch (l->ch) {
		case '|' : lexer_read_char(l); return S_PIPE;
		case ';' : lexer_read_char(l); return S_SEMI_COLUMN;
		case '(' : lexer_read_char(l); return S_SUB_OPEN;
		case ')' : lexer_read_char(l); return S_SUB_CLOSE;
		case '&' : lexer_read_char(l); return S_BG;
		default: printf("error separator type l->ch = %d %c\n", l->ch, l->ch); exit(EXIT_FAILURE);
	}
}

type_of_redirection get_redirection_type(Lexer_p l) {
	if (!ft_strncmp("&>>", &l->input[l->position], 3)) {
		lexer_read_x_char(l, 3);
		return R_DUP_BOTH_APPEND;
	}

	static const struct {
		char *str;
		type_of_redirection tag;
	} map[] = {
		{">>", R_APPEND}, {"<<", R_HERE_DOC},
		{"<&", R_DUP_IN}, {">&", R_DUP_OUT}, {"&>", R_DUP_BOTH},
	};

	for (size_t i = 0; i < sizeof(map) / sizeof(map[0]); i++) {
		if (!ft_strncmp(map[i].str, &l->input[l->position], 2)) {
			lexer_read_x_char(l, 2);
			return map[i].tag;
		}
	}

	switch (l->ch) {
		case '>': lexer_read_char(l); return R_OUTPUT;
		case '<': lexer_read_char(l); return R_INPUT;
		default: printf("Error lexing redirection !\n"); exit(EXIT_FAILURE);
	}
}
