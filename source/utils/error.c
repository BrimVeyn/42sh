/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 11:44:58 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/27 17:26:24 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "final_parser.h"
#include "colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void pretty_error(char *raw_token) {
	const Lex *lexer = lex_interface(LEX_OWN, NULL, NULL, NULL, NULL);
	char * const raw_input = ss_to_string(lexer->raw_input_ss);
	char buffer[MAX_WORD_LEN] = {0};

	int message_len = 0;
	if (*raw_token) {
		message_len = ft_sprintf(buffer, "%s:%ld:%ld: "C_BOLD C_BRIGHT_RED"error"C_RESET": syntax error near unexpected token: "C_BOLD C_WHITE"\'%s\'"C_RESET"\n", lexer->filename, lexer->pos.line, lexer->pos.column, raw_token);
	} else {
		message_len = ft_sprintf(buffer, "%s:%ld:%ld: "C_BOLD C_BRIGHT_RED"error"C_RESET": unexpected end of file\n", lexer->filename, lexer->pos.line, lexer->pos.column);
	}

	const int lineno_len = ft_sprintf(buffer, "  %ld", lexer->pos.line) - message_len;
	ft_sprintf(buffer, "  |  ");
	char * const line = get_line_x(raw_input, lexer->pos.line);
	int tab_count = 0;
	for (size_t i = 0; line[i]; i++) {
		if (line[i] == '\t') {
			tab_count += 1;
		}
	}
	ft_sprintf(buffer, "%s\n", line);
	for (int i = 0; i < lineno_len; i++) {
		ft_sprintf(buffer, " ");
	}
	ft_sprintf(buffer, "  |  ");
	for (size_t i = 0; i < (lexer->pos.column - (lexer->pos.column != 0) * 1); i++) {
		if (tab_count) {
			ft_sprintf(buffer, "\t");
			tab_count -= 1;
		} else {
			ft_sprintf(buffer, " ");
		}
	}
	ft_sprintf(buffer, C_BOLD C_BRIGHT_RED"^"C_RESET"\n");
	ft_dprintf(STDERR_FILENO, buffer);
	FREE_POINTERS(line);
}

void fatal(const char * const msg, const int exit_code) {
	ft_dprintf(STDERR_FILENO, "Fatal: %s\n", msg);
	gc(GC_CLEANUP, GC_ALL);
	exit(exit_code);
}

void error(const char * const msg, const int exit_code) {
	ft_dprintf(STDERR_FILENO, "%s\n", msg);
	g_exitno = exit_code;
}
