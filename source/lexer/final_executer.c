/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_executer.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 16:35:41 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/15 17:40:06 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"
#include "utils.h"
#include "colors.h"
#include "libft.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

void dup_input_and_close(int fd) {
	secure_dup2(fd, STDIN_FILENO);
	close(fd);
}

void create_pipe(bool *hadPipe, int *pipefd) {
	(*hadPipe) = true;
	secure_pipe2(pipefd, O_CLOEXEC);
	secure_dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[1]);
}

#define IS_CHILD(pid) ((pid == 0) ? true : false)

void resolve_bine(SimpleCommandP *command, Vars *shell_vars) {
	if (!command || !command->word_list->data[0]) {
		return ;
	}
	char *maybe_bin = hash_interface(HASH_FIND, command->word_list->data[0], shell_vars);
	if (is_builtin(command->word_list->data[0])) {
		return ;
	}
	if (maybe_bin) {
		hash_interface(HASH_ADD_USED, command->word_list->data[0], shell_vars);
		command->word_list->data[0] = maybe_bin;
	} else {
		bool absolute = false;
		maybe_bin = find_bin_location(command->word_list->data[0], shell_vars->env, &absolute);
		if (maybe_bin) {
			if (absolute == false)
				hash_interface(HASH_ADD_USED, command->word_list->data[0], shell_vars);
			command->word_list->data[0] = maybe_bin;
		} else {
			command->word_list->data[0] = NULL;
		}
	}
}

#define MAX_FD 1024
#include <sys/stat.h>

bool check_filepath(char *file_path, TokenType mode) {
	struct stat file_stat;
	if (stat(file_path, &file_stat) != -1) {

		if (S_ISDIR(file_stat.st_mode)) {
			dprintf(2, "42sh: %s: Is a directory\n", file_path);
			return (false);
		}
		if (file_stat.st_mode & S_IXUSR) {
			return file_path;
		}
		else {
			dprintf(2, "42sh: %s: Permission Denied\n", file_path);
			return (false);
		}
	} else {
		if (mode == LESS) {
			dprintf(2, "42sh: %s: No such file or directory\n", file_path);
			return false;
        }
		if (mode == GREAT)
			return true;
	}
	return true;
}

bool redirect_ios(RedirectionL *redir_list) {
	if (!redir_list) {
		return true;
	}
	print_redir_list(redir_list);

	for (size_t i = 0; i < redir_list->size; i++) {
		const RedirectionP *redir = redir_list->data[i];
		const bool has_io_number = (redir->io_number != NULL);
		int io_number = (has_io_number) ? ft_atoi(redir->io_number) : -1;
		if (io_number >= MAX_FD) {
			dprintf(2, "42sh: %d: Bad file descriptor\n", io_number); 
			g_exitno = 1;
			return false;
		}
		//TODO: error managment when open fails
		if (!check_filepath(redir->filename, redir->type)) {
			g_exitno = 1;
			return false;
		}
		switch (redir->type) {
			case (GREAT): { //>
				const int open_flags = (O_CREAT | O_TRUNC | O_WRONLY);
				const int fd = open(redir->filename, open_flags, 0664);
				int dup_to = STDOUT_FILENO;
				if (io_number != -1)
					dup_to = io_number;
				if (!secure_dup2(fd, dup_to))
					return false;
				close(fd);
				break;
			}
			case (DGREAT): { //>>
				const int open_flags = (O_CREAT | O_APPEND);
				const int fd = open(redir->filename, open_flags, 0664);
				int dup_to = STDOUT_FILENO;
				if (io_number != -1)
					dup_to = io_number;
				if (!secure_dup2(fd, dup_to))
					return false;
				close(fd);
				break;
			}
			case (LESSGREAT): { //<>
				const int open_flags = (O_CREAT | O_TRUNC | O_RDWR);
				const int fd = open(redir->filename, open_flags, 0664);
				if (!secure_dup2(fd, STDOUT_FILENO) || !secure_dup2(fd, STDIN_FILENO))
					return false;
				close(fd);
				break;
			}
			case (LESS): { //<
				const int open_flags = (O_RDONLY);
				int dup_to = STDIN_FILENO;
				const int fd = open(redir->filename, open_flags, 0664);
				if (io_number != -1) {
					dup_to = io_number;
				}
				if (!secure_dup2(fd, dup_to))
					return false;
				close(fd);
				break;
			}
			default: break;
		}
	}
	return true;
}

void execute_simple_command(CommandP *command, int *pipefd, Vars *shell_vars) {
	SimpleCommandP *simple_command = command->simple_command;
	resolve_bine(simple_command, shell_vars);
	if (!redirect_ios(simple_command->redir_list)) {
		return ;
	}

	dprintf(2, C_RED"-------------------------------------------"C_RESET"\n");
	print_simple_command(simple_command);
	dprintf(2, C_RED"-------------------------------------------"C_RESET"\n");

	execve(simple_command->word_list->data[0], simple_command->word_list->data, shell_vars->env->data);
	if (pipefd) {
		close(pipefd[0]);
		close(pipefd[1]);
	}
}

char *boolStr(bool bobo) {
	return (bobo == true) ? "TRUE" : "FALSE";
}

void execute_list(ListP *list, bool background, Vars *shell_vars);

void execute_subshell(CommandP *command, Vars *shell_vars) {
	ListP *subshell = command->subshell;
	if (!redirect_ios(command->redir_list)) {
		return ;
	}
	//FIX: add background trigger
	while (subshell) {
		execute_list(subshell, false, shell_vars);
		subshell = subshell->next;
	}
}

void execute_brace_group(CommandP *command, Vars *shell_vars) {
	ListP *command_group = command->brace_group;
	if (!redirect_ios(command->redir_list)) {
		return ;
	}
	//FIX: add background trigger, is it really there or inside execute_list that i'll be resolved ?
	while (command_group) {
		execute_list(command_group, false, shell_vars);
		command_group = command_group->next;
	}
}

void execute_if_clause(CommandP *command, Vars *shell_vars) {
	IFClauseP *if_clause = command->if_clause;

	size_t i = 0;
	for (; i < if_clause->conditions->size; i++) {
		ListP *condition = if_clause->conditions->data[i];
		execute_list(condition, false, shell_vars);
		if (g_exitno == 0) {
			ListP *body = if_clause->bodies->data[i];
			execute_list(body, false, shell_vars);
			return ;
		}
	}
	if (i < if_clause->bodies->size) {
		ListP *else_body = if_clause->bodies->data[i];
		execute_list(else_body, false, shell_vars);
	}
}

void execute_while_clause(CommandP *command, Vars *shell_vars) {
	WhileClauseP *while_clause = command->while_clause;
	while (execute_list(while_clause->condition, false, shell_vars), g_exitno == 0) {
		execute_list(while_clause->body, false, shell_vars);
	}
}

#define NO_WAIT 0
#define WAIT 1

int execute_single_command(AndOrP *job, bool background, Vars *shell_vars) {
	PipeLineP *process = job->pipeline;
	CommandP *command = job->pipeline->command;
	switch (command->type) {
		case Simple_Command: {
			resolve_bine(command->simple_command, shell_vars);
			if (is_builtin(command->simple_command->word_list->data[0])) {
				if (background) {
					pid_t pid = fork();
					if (IS_CHILD(pid)) {
						close_all_fds();
						execute_builtin(command->simple_command, shell_vars);
						exit(g_exitno);
					} else {
						process->pid = pid;
						if (!job->pgid)
							job->pgid = pid;
						setpgid(pid, job->pgid);
					}
				return WAIT;
				} else {
					execute_builtin(command->simple_command, shell_vars);
					return NO_WAIT;
				}
			} else {
				pid_t pid = fork();
				if (IS_CHILD(pid)) {
					close_all_fds();
					execute_simple_command(command, NULL, shell_vars);
					exit(g_exitno);
				} else {
					process->pid = pid;
					if (!job->pgid)
						job->pgid = pid;
					setpgid(pid, job->pgid);
				}
				return WAIT;
			}
		}
		case Subshell: {
			pid_t pid = fork();
			if (IS_CHILD(pid)) {
				close_all_fds();
				execute_subshell(command, shell_vars);
				exit(g_exitno);
			} else {
				process->pid = pid;
				if (!job->pgid)
					job->pgid = pid;
				setpgid(pid, job->pgid);
			}
			return WAIT;
		}
		case Brace_Group: { execute_brace_group(process->command, shell_vars); return NO_WAIT; }
		case If_Clause: { execute_if_clause(process->command, shell_vars); return NO_WAIT; }
		case While_Clause: { execute_while_clause(process->command, shell_vars);  return NO_WAIT; }
		case Case_Clause: { break; }
		case For_Clause: { break; }
		default: {};
	}
	return ERROR;
}

int execute_pipeline(AndOrP *job, bool background, Vars *shell_vars) {
	(void)background;
	PipeLineP *process = job->pipeline;
	int saved_fds[] = {
		[STDIN_FILENO] = dup(STDIN_FILENO),
		[STDOUT_FILENO] = dup(STDOUT_FILENO),
		[STDERR_FILENO] = dup(STDERR_FILENO),
	};

	bool hadPipe = false;
	int pipefd[2] = {-1, -1};
	const bool piped = (process->next != NULL);
	dprintf(2, "piped: %s\n", boolStr(piped));

	if (!piped) {
		return execute_single_command(job, background, shell_vars);
	}

	while (process) {
		const bool hasPipe = (process->next != NULL);

		dprintf(2, "hasPipe: %s | hadPipe: %s\n", boolStr(hasPipe), boolStr(hadPipe));

		if (hadPipe) dup_input_and_close(pipefd[0]);
		if (hasPipe) create_pipe(&hadPipe, pipefd);

		pid_t pid = fork();

		if (IS_CHILD(pid)) {
			close_all_fds();

			switch (process->command->type) {
				case Simple_Command: { execute_simple_command(process->command, pipefd, shell_vars); exit(g_exitno); break; }
				case Subshell: { execute_subshell(process->command, shell_vars); exit(g_exitno); break; }
				case Brace_Group: { execute_brace_group(process->command, shell_vars); exit(g_exitno); break; }
				case If_Clause: { execute_if_clause(process->command, shell_vars); exit(g_exitno); break ; }
				case While_Clause: { execute_while_clause(process->command, shell_vars); exit(g_exitno); break ; }
				case Case_Clause: { break; }
				case For_Clause: { break; }
				default: break;
			}
		} else {
			process->pid = pid;
			if (!job->pgid)
				job->pgid = pid;
			setpgid(pid, job->pgid);
		}
		process = process->next;

		dup2(saved_fds[STDIN_FILENO], STDIN_FILENO);
		dup2(saved_fds[STDOUT_FILENO], STDOUT_FILENO);
		dup2(saved_fds[STDERR_FILENO], STDERR_FILENO);
	}
	close_saved_fds(saved_fds);
	return WAIT;
}

void execute_list(ListP *list, bool background, Vars *shell_vars) {
	AndOrP *andor_head = list->and_or;
	ShellInfos *shell_infos = shell(SHELL_GET);

	while (andor_head) {
		int wait_status;
		const TokenType separator = andor_head->separator; (void)separator;
		wait_status = execute_pipeline(andor_head, background, shell_vars);

		if (wait_status == WAIT) {
			if (!shell_infos->interactive)
				job_wait(andor_head);
			else if (background)
				put_job_background(list->and_or);
			else
				put_job_foreground(list->and_or, false);
		}

		bool skip = (
			(separator == AND_IF && g_exitno != 0) 
			||
			(separator == OR_IF && g_exitno == 0)
		);
		dprintf(2, "SKIP: %s\n", boolStr(skip));
		if (g_exitno == 0) {
			dprintf(2, C_EMERALD"SUCCESS %d"C_RESET"\n", wait_status);
		} else {
			dprintf(2, C_VIOLET"FAILED %d"C_RESET"\n", wait_status);
		}
		andor_head = andor_head->next;
	}


}


void execute_complete_command(CompleteCommandP *complete_command, Vars *shell_vars) {
	ListP *list_head = complete_command->list;
	ShellInfos *shell_infos = shell(SHELL_GET);
	// dprintf(2, "INTERFACTIVE: %d | %s\n", shell_infos->interactive, boolStr(shell_infos->interactive));

	const bool background = (shell_infos->interactive && complete_command->separator == AMPER);

	while (list_head) {
		execute_list(list_head, background, shell_vars);
		list_head = list_head->next;
	}
	dprintf(2, "END OF EXECUTION\n");
	return ;
}