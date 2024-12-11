/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_substitution.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 11:15:03 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/11 10:50:32 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "utils.h" 
#include "final_parser.h"

#include <limits.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

char *parser_command_substitution(char *const str, Vars *const shell_vars, int * const error) {
	ShellInfos *shell_infos = shell(SHELL_GET);

    char file_name[] = "/tmp/command_sub_XXXXXX";

    int output_fd = mkstemp(file_name);
	if (output_fd == -1)
		_fatal("mkstemp: failed to create command_sub", 1);

	const int STDOUT_SAVE = dup(STDOUT_FILENO);
	da_push(g_fdSet, STDOUT_SAVE);

	if (dup2(output_fd, STDOUT_FILENO) == -1)
		_fatal("dup2: failed", 1);
	close(output_fd);

	bool was_interactive = shell_infos->interactive;
	if (was_interactive) shell_infos->interactive = false;

	if (parse_input(str, "command_sub", shell_vars) == ERR)
		(*error) = 1;

	if (was_interactive) shell_infos->interactive = true;

	if (dup2(STDOUT_SAVE, STDOUT_FILENO) == -1)
		_fatal("dup2: failed", 1);
	remove_fd_set(STDOUT_SAVE);
	close(STDOUT_SAVE);

	output_fd = open(file_name, O_RDONLY, 0644);
	if (output_fd == -1)
		_fatal("open: command_sub file not found", 1);

	char * const result = read_whole_file(output_fd);
	close(output_fd); 
	unlink(file_name);
	
	return result;
}

