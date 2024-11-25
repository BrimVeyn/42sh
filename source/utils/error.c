/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 11:44:58 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/25 17:02:29 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "final_parser.h"
#include "colors.h"
#include <stdlib.h>

void pretty_error(char *raw_token) {
	Lex *lexer = lex_interface(LEX_OWN, NULL, NULL, NULL);
	char buffer[MAX_WORD_LEN] = {0};
	int message_len = ft_sprintf(buffer, "%s:%ld:%ld: "C_BOLD C_BRIGHT_RED"error"C_RESET": syntax error near unexpected token: "C_BOLD C_WHITE"\'%s\'"C_RESET"\n", lexer->filename, lexer->line, lexer->column, raw_token);
	int lineno_len = ft_sprintf(buffer, "  %ld", lexer->line) - message_len;
	ft_sprintf(buffer, "  |  ");
	char *line = get_line_x(lexer->raw_input, lexer->line);
	int tab_count = 0;
	for (size_t i = 0; line[i]; i++) {
		if (line[i] == '\t') {
			tab_count += 1;
		}
	}
	ft_sprintf(buffer, "%s\n", line);
	for(int i = 0; i < lineno_len; i++) {
		ft_sprintf(buffer, " ");
	}
	ft_sprintf(buffer, "  |  ");
	for (size_t i = 0; i < lexer->column - 1; i++) {
		if (tab_count) {
			ft_sprintf(buffer, "\t");
			tab_count -= 1;
		} else {
			ft_sprintf(buffer, " ");
		}
	}
	ft_sprintf(buffer, C_BOLD C_BRIGHT_RED"^"C_RESET"\n");
	ft_dprintf(2, buffer);
	FREE_POINTERS(line);
}

void fatal(const char * const msg, const int exit_code) {
	ft_dprintf(2, "Fatal: %s\n", msg);
	gc(GC_CLEANUP, GC_ALL);
	exit(exit_code);
}

void error(const char * const msg, const int exit_code) {
	ft_dprintf(2, "%s\n", msg);
	g_exitno = exit_code;
}
