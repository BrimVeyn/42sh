/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command_substitution.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:38:13 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/14 22:26:26 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

static int get_command_sub_range_end(char *str, int i) {
	const int str_len = ft_strlen(str);

	i += 2;
	while (i < str_len && str[i]) {
		if (!ft_strncmp(&str[i], "$(", 2)) {
			i = get_command_sub_range_end(str, i);
			if (i == -1) return i;
			else i += 1;
		}
		if (str[i] == ')') {
			return i;
		}
		i += 1;
	}
	return -1;
}

static Range get_command_sub_range(char *str) {
	Range self = {
		.start = -1,
		.end = -1,
	};

	self.start = ft_strstr(str, "$(");
	if (self.start != -1)
		self.end = get_command_sub_range_end(str, self.start);

	return self;
}

char *parser_get_variable_value2(char *name, char **env){
	for (int i = 0; env[i]; i++){
		if (ft_strncmp(env[i], gc_add(ft_strjoin(name, "="), GC_GENERAL), ft_strlen(name) + 1) == 0){
			return ft_strdup(env[i] + ft_strlen(name) + 1);
		}
	}
	return ft_strdup("");
}

TokenList *ft_token_split(const char *str, const char *ifs) {
	TokenList *tl = token_list_init();
	const uint16_t str_len = ft_strlen(str);

	for (uint16_t i = 0; i < str_len; i++) {
		Token *word = (Token *) gc_add(token_empty_init(), GC_SUBSHELL);
		word->tag = T_WORD;
		token_word_init(word);

		const int start = i;
		while (i < str_len && !ft_strchr(ifs, str[i])) {
			i++;
		}
		const int end = i;

		if (start == end)
			word->w_infix = gc_add(ft_strdup(""), GC_SUBSHELL);
		else
			word->w_infix = gc_add(ft_substr(str, start, end - start), GC_SUBSHELL);

		token_list_add(tl, word);
	}
	return tl;
}

TokenList *word_splitter(const char *str, char **env) {
	char *IFS_value = parser_get_variable_value2("IFS", env);
	if (!ft_strcmp("", IFS_value)) {
		free(IFS_value);
		IFS_value = ft_strdup("\n\t ");
	}

	TokenList *list = ft_token_split(str, IFS_value);
	if (list->size == 0) {
		Token *word = (Token *) gc_add(token_empty_init(), GC_SUBSHELL);
		word->tag = T_WORD;
		token_word_init(word);
		word->w_infix = (char *) gc_add(ft_strdup(""), GC_SUBSHELL);
		token_list_add(list, word);
	}
	free(IFS_value);
	return list;
}

bool parser_command_substitution(TokenList *tl, char **env) {
	static int COMMAND_SUB_NO = 0;
	int i = 0;

	while(i < tl->size) {
		Token *elem = tl->t[i];
		if (elem->tag == T_WORD) {
			const Range range = get_command_sub_range(elem->w_infix);

			if (range.start == -1 || range.end == -1) {
				if (range.start != -1 && range.end == -1) {
					dprintf(2, UNCLOSED_COMMAND_SUB_STR);
					g_exitno = 126;
					return false;
				}
				i += 1;
				continue;
			}

			char infix[MAX_WORD_LEN] = {0};
			ft_memcpy(infix, &elem->w_infix[range.start + 1], range.end - range.start);
			// dprintf(2, "INFIX: %s\n", infix);

			char file_name[MAX_FILENAME_LEN] = {0};
			ft_sprintf(file_name, "/tmp/command_sub_%d", COMMAND_SUB_NO++);

			int output_fd = open(file_name, O_CREAT | O_TRUNC | O_WRONLY | O_CLOEXEC, 0644);
			if (output_fd == -1) {
				printf("failed\n");
				exit(EXIT_FAILURE);
			}

			int STDOUT_SAVE = dup(STDOUT_FILENO);
			dup2(output_fd, STDOUT_FILENO);
			close(output_fd);

			Lexer_p lexer = lexer_init(infix);
			TokenList *tokens = lexer_lex_all(lexer);
			// tokenListToString(tokens);
			if (lexer_syntax_error(tokens)) continue; 
			heredoc_detector(tokens);
			Node *AST = ast_build(tokens);
			ast_execute(AST, env);

			dup2(STDOUT_SAVE, STDOUT_FILENO);
			close(STDOUT_SAVE);

			output_fd = open(file_name, O_RDONLY, 0644);
			if (output_fd == -1) {
				printf("failed 2\n");
				exit(EXIT_FAILURE);
			}

			char *result = read_whole_file(output_fd);
			close(output_fd);
			unlink(file_name);

			TokenList *word_splitted_result = word_splitter(result, env);
			free(result);
			// tokenListToString(word_splitted_result);
			token_list_remove(tl, i);
			token_list_insert_list(tl, word_splitted_result, i);

			char *prefix = ft_substr(elem->w_infix, 0, range.start);
			char *postfix = ft_substr(elem->w_infix, range.end + 1, ft_strlen(elem->w_infix) - range.end);

			const Range new_word = {
				.start = i,
				.end = i + word_splitted_result->size - 1,
			};

			char *old_word_start = tl->t[new_word.start]->w_infix;
			char *old_word_end = tl->t[new_word.end]->w_infix;

			tl->t[new_word.start]->w_infix = (char *) gc_add(ft_strjoin(prefix, tl->t[new_word.start]->w_infix), GC_SUBSHELL);
			tl->t[new_word.end]->w_infix = (char *) gc_add(ft_strjoin(tl->t[new_word.end]->w_infix, postfix), GC_SUBSHELL);

			gc_free(old_word_start, GC_SUBSHELL);
			gc_free(old_word_end, GC_SUBSHELL);
			FREE_POINTERS(prefix, postfix);
			// dprintf(2, "s: %s\n", tl->t[i]->w_infix);
			// dprintf(2, "e: %s\n", tl->t[i + word_splitted_result->size - 1]->w_infix);
		} else
			i += 1;
	}
	return true;
}

