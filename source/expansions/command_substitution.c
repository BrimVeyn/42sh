/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_substitution.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 10:08:11 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/09 15:01:15 by bvan-pae         ###   ########.fr       */
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

bool is_command_sub = 0;

char *parser_command_substitution(char *str, Vars *shell_vars) {

    char file_name[] = "/tmp/command_sub_XXXXXX";
    int output_fd = mkstemp(file_name);
	if (output_fd == -1)
		_fatal("mkstemp: failed to create command_sub", 1);

	const int STDOUT_SAVE = dup(STDOUT_FILENO);
	da_push(g_fdSet, STDOUT_SAVE);

	dup2(output_fd, STDOUT_FILENO);
	close(output_fd);

	is_command_sub = true;
	parse_input(str, "command_sub", shell_vars);
	is_command_sub = false;

	dup2(STDOUT_SAVE, STDOUT_FILENO); 
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

