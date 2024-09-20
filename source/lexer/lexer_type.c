/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_type.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 13:01:17 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/04 14:09:40 by bvan-pae         ###   ########.fr       */
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
	if (l->ch == '\0') {
		lexer_read_char(l);
		return S_EOF;
	}
	else if (l->ch == '\n') {
		lexer_read_char(l);
		return S_NEWLINE;
	}
	else if (!ft_strncmp("$(", &l->input[l->position], 2)) {
		lexer_read_x_char(l, 2);
		return S_CMD_SUB;
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
		return S_SUB_CLOSE;
	}
	else if (l->ch == '(') {
		lexer_read_char(l);
		return S_SUB_OPEN;
	}
	else if (l->ch == '\"') {
		lexer_read_char(l);
		return S_DQ;
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
