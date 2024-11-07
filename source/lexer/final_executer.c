/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_executer.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 15:02:22 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/07 17:09:01 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define IS_CHILD(pid) ((pid == 0) ? true : false)

void resolve_bine(SimpleCommandP *command, Vars *shell_vars) {
	if (!command || !command->bin) {
		return ;
	}
	char *maybe_bin = hash_interface(HASH_FIND, command->bin, shell_vars);
	if (is_builtin(command->bin)) {
		return ;
	}
	if (maybe_bin) {
		hash_interface(HASH_ADD_USED, command->bin, shell_vars);
		command->bin = maybe_bin;
	} else {
		bool absolute = false;
		maybe_bin = find_bin_location(command->bin, shell_vars->env, &absolute);
		if (maybe_bin) {
			if (absolute == false)
				hash_interface(HASH_ADD_USED, command->bin, shell_vars);
			command->bin = maybe_bin;
		} else {
			command->bin = NULL;
		}
	}
}

void execute_simple_command(CommandP *command, bool background, int *pipefd, Vars *shell_vars) {
	(void)background;
	SimpleCommandP *simple_command = command->simple_command;
	simple_command->bin = simple_command->word_list->data[0];

	print_simple_command(simple_command);
	(void)pipefd;
	resolve_bine(simple_command, shell_vars);

	pid_t pid = fork();

	if (IS_CHILD(pid)) {

	} else {

	}
}

void execute_command(CommandP *command, bool background, int *pipefd, Vars *shell_vars) {

	switch (command->type) {
		case Simple_Command: {
			execute_simple_command(command, background, pipefd, shell_vars);
			break;
		}
		default: break;
	}
}


void execute_pipeline(PipeLineP *pipeline, bool background, Vars *shell_vars) {
	PipeLineP *pipeline_head = pipeline;
	int saved_fds[] = {
		[STDIN_FILENO] = dup(STDIN_FILENO),
		[STDOUT_FILENO] = dup(STDOUT_FILENO),
		[STDERR_FILENO] = dup(STDERR_FILENO),
	};

	(void)saved_fds;
	bool hadPipe = false;
	int pipefd[2] = {-1, -1};

	while (pipeline_head) {
		const bool hasPipe = (pipeline_head->next != NULL);

		if (hadPipe) {
			secure_dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
		}

		if (hasPipe) {
			hadPipe = true;
			secure_pipe2(pipefd, O_CLOEXEC);
			secure_dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);
		}

		execute_command(pipeline_head->command, background, pipefd, shell_vars);
		pipeline_head = pipeline_head->next;
	}
}

void execute_andor(AndOrP *and_or, bool background, Vars *shell_vars) {
	AndOrP *andor_head = and_or;

	while (andor_head) {
		const TokenType separator = andor_head->separator;
		(void)separator;
		execute_pipeline(andor_head->pipeline, background, shell_vars);
		andor_head = andor_head->next;
	}
}

void execute_complete_command(CompleteCommandP *complete_command, Vars *shell_vars) {
	ListP *list_head = complete_command->list;

	while (list_head) {
		bool background = (list_head->separator == AMPER);
		execute_andor(list_head->and_or, background, shell_vars);
		list_head = list_head->next;
	}
	return ;
}
