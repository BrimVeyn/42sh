/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command_substitution.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 10:08:11 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/23 22:46:21 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h" 
#include "ft_regex.h"

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

static void clean_sub(void) {
	gc(GC_CLEANUP, GC_ENV);
	gc(GC_CLEANUP, GC_SUBSHELL);
	gc(GC_CLEANUP, GC_READLINE);
	free(((Garbage *)gc(GC_GET))[GC_GENERAL].garbage);
	exit(g_exitno);
}

bool execute_command_sub(char *input, Vars *shell_vars) {
	pid_t pid = fork();
	if (!pid) {
		//FIX: uncomment next line
		// parse_input(input, "command_sub", shell_vars);
		(void)input;(void)shell_vars;
		clean_sub();
		// TokenList *tokens = lexer_lex_all(input);
		// if (lexer_syntax_error(tokens))
		// 	clean_sub();
		// heredoc_detector(tokens, shell_vars);
		// Node *AST = ast_build(tokens);
		// ast_execute(AST, shell_vars, true);
		// clean_sub();
	} else {
		//TODO: must be backgrounable
		//parsed in the children ?
		int status;
		waitpid(pid, &status, 0);
		g_exitno = WEXITSTATUS(status);
	}
	return true;
}

bool is_variable_assignment(char *str) {
	if (regex_match("[_a-zA-Z][_a-zA-Z0-9]*=", str).is_found) {
		return true;
	}
	return false;
}

char *parser_command_substitution(char *str, Vars *shell_vars) {
	static int COMMAND_SUB_NO = 0;

	char file_name[MAX_FILENAME_LEN] = {0};
	ft_sprintf(file_name, "/tmp/command_sub_%d", COMMAND_SUB_NO++);

	int output_fd = open(file_name, O_CREAT | O_TRUNC | O_WRONLY | O_CLOEXEC, 0644);
	int STDOUT_SAVE = dup(STDOUT_FILENO);

	dup2(output_fd, STDOUT_FILENO); close(output_fd);
	if (!execute_command_sub(str, shell_vars)) { 
		dup2(STDOUT_SAVE, STDOUT_FILENO);
		return NULL;
	}
	dup2(STDOUT_SAVE, STDOUT_FILENO); close(STDOUT_SAVE);

	output_fd = open(file_name, O_RDONLY, 0644);
	if (output_fd == -1) {
		printf("failed 2\n");
		exit(EXIT_FAILURE);
	}

	char *result = read_whole_file(output_fd);
	close(output_fd); unlink(file_name);
	
	return result;
}

