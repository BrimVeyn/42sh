/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 11:17:58 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/13 16:09:22 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "utils.h"
#include "final_parser.h"
#include "colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void pretty_error(const Lex *const lexer, char *raw_token) {
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
	g_exitno = 2;
}

#include <stdarg.h>
#include <stdio.h>

void _debug(const char *format, ...) {
    va_list args;
    va_start(args, format);
	if (g_debug)
		vdprintf(2, format, args);
    va_end(args);
}

void fatal(const char * const msg, const int line, const char *filename, const int exit_code) {
	perror("fatal");
    if (msg)
        ft_dprintf(STDERR_FILENO, "%s:%d 42sh: fatal: %s\n", filename, line, msg);
    close_fd_set(FD_ALL);
    gc(GC_CLEANUP, GC_ALL);
    exit(exit_code);
}

void error(const char * const msg, const int exit_code) {
	ft_dprintf(STDERR_FILENO, "%s\n", msg);
	g_exitno = exit_code;
}

void exit_clean(void) {
	close_fd_set(FD_ALL);
	gc(GC_CLEANUP, GC_ALL);
	exit(g_exitno);
}
