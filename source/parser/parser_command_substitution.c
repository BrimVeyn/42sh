/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command_substitution.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:38:13 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/16 13:26:49 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"
#include <stdint.h>

typedef struct {
	int start;
	int end;
} Range;

int ft_strstr(const char *haystack, const char *needle) {
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

int ft_strrstr(const char *haystack, const char *needle) {
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

char *read_whole_file(int fd) {
	size_t buffer_size = 10;
	char buffer[10] = {0};

	size_t result_size = 0;
	size_t result_capacity = 10;
	size_t bytes_read = 0;
	char *result = ft_calloc(buffer_size, sizeof(char));

	while ((bytes_read = read(fd, buffer, buffer_size)) != 0) {
		buffer[bytes_read] = '\0';
		if (result_size + bytes_read >= result_capacity) {
			result_capacity *= 2;
			char *tmp = result;
			result = ft_realloc(result, result_size, result_capacity, sizeof(char));
			free(tmp);
		}
		ft_memcpy(result + result_size, buffer, bytes_read);
		result_size += bytes_read;
		result[result_size] = '\0';
	}
	if (result[result_size - 1] == '\n')
		result[result_size - 1] = '\0';
	return result;
}

TokenList *ft_token_split(char *str, char *ifs) {
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
		if (start == end) {
			word->w_infix = gc_add(ft_strdup(""), GC_SUBSHELL);
		}
		word->w_infix = gc_add(ft_substr(str, start, end - start), GC_SUBSHELL);
		token_list_add(tl, word);
	}
	return tl;
}

TokenList *word_splitter(char *str, StringList *env) {
	char *IFS_value = string_list_get_value_with_id(env, "IFS");
	if (!IFS_value) {
		free(IFS_value);
		IFS_value = ft_strdup("\n\t ");
	}

	TokenList *list = ft_token_split(str, IFS_value);
	// tokenToStringAll(list);
	return list;
}

bool parser_command_substitution(TokenList *tl, StringList *env, int *saved_fds) {
	static int COMMAND_SUB_NO = 0;
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

			char file_name[1024] = {0};
			sprintf(file_name, "/tmp/command_sub_%d", COMMAND_SUB_NO);
			int output_fd = open(file_name, O_CREAT | O_TRUNC | O_WRONLY | O_CLOEXEC, 0644);
			if (output_fd == -1) {
				printf("failed\n");
				exit(EXIT_FAILURE);
			}

			int STDOUT_SAVE = dup(STDOUT_FILENO);
			(void) saved_fds;
			dup2(output_fd, STDOUT_FILENO);
			close(output_fd);

			Lexer_p lexer = lexer_init(infix);
			TokenList *tokens = lexer_lex_all(lexer);
			// tokenToStringAll(tokens);
			free(infix);
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
			// char *resul2 = ft_strjoin(prefix, result);
			// char *result3 = ft_strjoing(result, postfix);
			close(output_fd);
			TokenList *word_splitted_result = word_splitter(result, env);
			// dprintf(2, C_RED"%s\n"C_RESET, result);
			free(result);
			// tokenToStringAll(word_splitted_result);
			token_list_remove(tl, i);
			token_list_insert_list(tl, word_splitted_result, i);
			gc_free(word_splitted_result, GC_SUBSHELL);

			// const char *prefix = ft_substr(elem->w_infix, 0, range.start);
			// const char *postfix = ft_substr(elem->w_infix, range.end + 1, ft_strlen(elem->w_infix) - range.end);
			// (void) prefix;
			// (void) postfix;
			// dprintf(2, "pre: |%s|, post: |%s|\n", prefix, postfix);
			COMMAND_SUB_NO += 1;
		}
	}
	return true;
}

