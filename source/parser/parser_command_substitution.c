/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command_substitution.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:31:07 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/24 13:04:07 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser.h"
#include "../../libftprintf/header/libft.h"
#include "../../include/utils.h" 
#include "../../include/ast.h" 
#include "exec.h"

#include <unistd.h>
#include <fcntl.h>

int get_command_sub_range_end(char *str, int *i) {
	(*i) += 2; // skip '$('
	
	while (str[*i] && str[*i] != ')') {
		if (!ft_strncmp("$(", &str[*i], 2)) {
			get_command_sub_range_end(str, i);
			if (*i == -1) return -1;
		}
		if (str[*i] == '(') {
			get_command_sub_range_end(str, i);
			if (*i == -1) return -1;
		}
		(*i)++;
	}
	if (str[*i] != ')') return -1;
	return *i;
}

static Range get_command_sub_range(char *str) {
	Range self = {
		.start = -1,
		.end = -1,
	};

	self.start = ft_strstr(str, "$(");
	if (self.start != -1 && !ft_strncmp(&str[self.start], "$((", 3)) {
		self.start = -1;
	}
	if (self.start != -1) {
		int start_copy = self.start;
		self.end = get_command_sub_range_end(str, &start_copy);
	}
	return self;
}

bool is_a_match(const Range range) {
	return (range.start == -1 || range.end == -1);
}

bool is_range_valid(const Range range, char *str) {
	if (range.start != -1 && range.end == -1) {
		dprintf(2, "%s", str);
		return false;
	}
	return true;
}

static bool execute_command_sub(char *input, Vars *shell_vars) {
	Lexer_p lexer = lexer_init(input);
	TokenList *tokens = lexer_lex_all(lexer);
	if (lexer_syntax_error(tokens))  {
		return false;
	}
	heredoc_detector(tokens);
	Node *AST = ast_build(tokens);
	ast_execute(AST, shell_vars);

	return true;
}

bool parser_command_substitution(TokenList *tokens, Vars *shell_vars) {
	static int COMMAND_SUB_NO = 0;
	bool error = false;
	int i = 0;

	while(i < tokens->size) {
		Token *elem = tokens->t[i];

		if (!is_word(tokens, &i)) {
			i += 1;
			continue;
		}

		const Range range = get_command_sub_range(elem->w_infix);
		if (is_a_match(range)) {
			if (!is_range_valid(range, UNCLOSED_COMMAND_SUB_STR)) {
				g_exitno = 126;
				return false;
			} else i += 1; continue;
		}

		char infix[MAX_WORD_LEN] = {0};
		ft_memcpy(infix, &elem->w_infix[range.start + 1], range.end - range.start);

		char file_name[MAX_FILENAME_LEN] = {0};
		ft_sprintf(file_name, "/tmp/command_sub_%d", COMMAND_SUB_NO++);

		int output_fd = open(file_name, O_CREAT | O_TRUNC | O_WRONLY | O_CLOEXEC, 0644);
		if (output_fd == -1) {
			printf("DUP failed in command sub what do we do ?\n");
			exit(EXIT_FAILURE);
		}

		int STDOUT_SAVE = dup(STDOUT_FILENO);

		dup2(output_fd, STDOUT_FILENO); close(output_fd);
		if (!execute_command_sub(infix, shell_vars)) {
			error = true;
		}
		dup2(STDOUT_SAVE, STDOUT_FILENO); close(STDOUT_SAVE);


		output_fd = open(file_name, O_RDONLY, 0644);
		if (output_fd == -1) {
			printf("failed 2\n");
			exit(EXIT_FAILURE);
		}

		char *result = read_whole_file(output_fd);
		close(output_fd); unlink(file_name);


		char *prefix = ft_substr(elem->w_infix, 0, range.start);
		char *postfix = ft_substr(elem->w_infix, range.end + 1, ft_strlen(elem->w_infix) - range.end);

		if (!ft_strlen(result) && !ft_strlen(prefix) && !ft_strlen(postfix)) {
			FREE_POINTERS(prefix, result, postfix);
			elem->w_infix = NULL;
			i += 1;
			continue;
		}

		parser_word_split(tokens, shell_vars, prefix, result, postfix, i);
		FREE_POINTERS(prefix, result, postfix);
		if (error) return false;
	}
	return true;
}

