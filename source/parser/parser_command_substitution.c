/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command_substitution.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:38:13 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/12 17:26:24 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"
#include <fcntl.h>
#include <unistd.h>

typedef struct {
	int start;
	int end;
} Range;

static int ft_strstr(char *haystack, char *needle) {
	if (!haystack || !needle) {
		return -1;
	}
	
	const int needle_len = ft_strlen(needle);
	int i = 0;

	while (haystack[i]) {
		if (!ft_strncmp(&haystack[i], needle, needle_len)) {
			return i;
		}
		i += 1;
	}
	return -1;
}

static int ft_strrstr(char *haystack, char *needle) {
	if (!haystack || !needle) {
		return -1;
	}
	
	const int needle_len = ft_strlen(needle);
	int i = ft_strlen(haystack) - 1;

	while (i >= 0) {
		if (!ft_strncmp(&haystack[i], needle, needle_len)) {
			return i;
		}
		i -= 1;
	}
	return -1;
}

Range get_command_sub_range(char *str) {
	return (const Range) {
		.start = ft_strstr(str, "$("),
		.end = ft_strrstr(str, ")"),
	};
}

bool parser_command_substitution(TokenList *tl, char **env, int *saved_fds) {
	for (int i = 0; i < tl->size; i++) {
		Token *elem = tl->t[i];
		if (elem->tag == T_WORD) {
			const Range range = get_command_sub_range(elem->w_infix);
			if (range.start == -1 || range.end == -1) {
				continue;
			}
			char *infix = ft_substr(elem->w_infix, range.start + 1, range.end - range.start);

			// dprintf(2, "infix: |%s|\n", infix);
			// dprintf(2, "s: %d, e: %d\n", range.start, range.end);

			int output_fd = open("/tmp/command_sub", O_CREAT | O_TRUNC | O_WRONLY | O_CLOEXEC, 0644);
			if (output_fd == -1) {
				printf("failed\n");
				exit(EXIT_FAILURE);
			}

			int STDOUT_SAVE = dup(STDOUT_FILENO);
			dup2(output_fd, STDOUT_FILENO);
			close(output_fd);
			close_saved_fds(saved_fds);

			Lexer_p lexer = lexer_init(infix);
			TokenList *tokens = lexer_lex_all(lexer);
			free(infix);
			if (lexer_syntax_error(tokens)) continue; 
			heredoc_detector(tokens);
			Node *AST = ast_build(tokens);
			ast_execute(AST, env);

			dup2(STDOUT_SAVE, STDOUT_FILENO);
			close(STDOUT_SAVE);

			output_fd = open("/tmp/command_sub", O_RDONLY, 0644);
			if (output_fd == -1) {
				printf("failed 2\n");
				exit(EXIT_FAILURE);
			}

			char *result = ft_strdup("");
			char *line = NULL;
			while ((line = gnl(output_fd)) != NULL) {
				char *oldresult = result;
				result = ft_strjoin(result, line);
				free(oldresult);
				free(line);
			}
			close(output_fd);

			const char *prefix = ft_substr(elem->w_infix, 0, range.start);
			const char *postfix = ft_substr(elem->w_infix, range.end + 1, ft_strlen(elem->w_infix) - range.end);
			// dprintf(2, "result: |%s|\n", result);
			// dprintf(2, "pre: |%s|, post: |%s|\n", prefix, postfix);
	
			char *temp = ft_strjoin(prefix, result);
			free(result);
			free((char *)prefix);
			gc_free(elem->w_infix, GC_SUBSHELL);
			elem->w_infix = (char *) gc_add(ft_strjoin(temp, postfix), GC_SUBSHELL);

			free((char *)postfix);
			free(temp);
		}
	}
	return true;
}
