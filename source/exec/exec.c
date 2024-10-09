/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 14:56:16 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/09 14:44:56 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "signals.h"
#include "utils.h"
#include "libft.h"

#include <readline/history.h>
#include <stdint.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

int g_exitno;

//LARBIN add FD validity check (>1023)
bool apply_redirect(const Redirection redirect) {
	int fd = redirect.fd;
	const int open_flag = (redirect.r_type == R_INPUT) * (O_RDONLY)
		+ (redirect.r_type == R_OUTPUT || redirect.r_type == R_DUP_BOTH) * (O_WRONLY | O_TRUNC | O_CREAT)
		+ (redirect.r_type == R_APPEND || redirect.r_type == R_DUP_BOTH_APPEND) * (O_WRONLY | O_CREAT | O_APPEND);

	int dup_on = (redirect.r_type == R_INPUT || redirect.r_type == R_DUP_IN) * STDIN_FILENO
		+ (redirect.r_type == R_OUTPUT || redirect.r_type == R_APPEND || redirect.r_type == R_DUP_OUT) * STDOUT_FILENO;

	if (redirect.prefix_fd != -1){
		dup_on = redirect.prefix_fd;
	}

	if (redirect.su_type == R_FILENAME && (fd = open(redirect.filename, open_flag, 0664)) == -1){
		return false;
	}

	if (dup_on >= 1024){
		dprintf(2, "42sh: %d: Bad file descriptor\n", dup_on);
		return false;
	}
	if (fd >= 1024){
		dprintf(2, "42sh: %d: Bad file descriptor\n", fd);
		return false;
	}

	if (redirect.r_type == R_DUP_BOTH || redirect.r_type == R_DUP_BOTH_APPEND) {
		if (!secure_dup2(fd, STDOUT_FILENO)) return false;
		if (!secure_dup2(fd, STDERR_FILENO)) return false;
	}
	else {
		if (!secure_dup2(fd, dup_on)) return false;
	}
	if (redirect.su_type == R_FILENAME){
		close(fd);
	}
	return true;
}

bool apply_all_redirect(RedirectionList *redirections) {
	for (int i = 0; redirections->r[i]; i++) {
		if (!apply_redirect(*redirections->r[i])) {
			return false;
		}
	}
	return true;
}

char *find_bin_location(char *bin, StringList *env){
	struct stat file_stat;
	if (stat(bin, &file_stat) != -1) {

		if (S_ISDIR(file_stat.st_mode)) {
			dprintf(2, "%s: Is a directory\n", bin);
			g_exitno = 126;
			return (NULL);
		}
		if (file_stat.st_mode & S_IXUSR) {
			return bin;
		}
		else {
			dprintf(2, "%s: Permission Denied\n", bin);
			g_exitno = 126;
			return (NULL);
		}

	}
	else if (stat(bin, &file_stat) == -1 && there_is_slash(bin)) {
		dprintf(2, "%s: No such file or directory\n", bin);
		g_exitno = 127;
		return (NULL);
	}

	char **path = ft_split(string_list_get_value(env, "PATH"), ':');
	for (int i = 0; path[i]; i++) {
		char *bin_with_path = ft_strjoin(path[i], (char *)gc(GC_ADD, ft_strjoin("/",bin), GC_SUBSHELL));

		struct stat file_stat;
		if (stat(bin_with_path, &file_stat) != -1){
			if (file_stat.st_mode & S_IXUSR){
				free_charchar(path);
				return (char *)gc(GC_ADD, bin_with_path, GC_SUBSHELL);
			}
		}
		free(bin_with_path);
		bin_with_path = NULL;
	}
	free_charchar(path);
	dprintf(2, "%s: command not found\n", bin);
	g_exitno = 127;
	return NULL;
}

bool exec_simple_command(const SimpleCommand *command, Vars *shell_vars) {
	if (apply_all_redirect(command->redir_list)) {
		if (!command->bin) {
			return false;
		}
		if (builtin_executer(command, shell_vars))
			return false;
		secure_execve(command->bin, command->args, shell_vars->env->data);
	} else return false;
	return true;
}

void close_saved_fds(int *saved_fds) {
	if (saved_fds[STDIN_FILENO] != -1) {
		close(saved_fds[STDIN_FILENO]);
		saved_fds[STDIN_FILENO] = -1;
	}	
	if (saved_fds[STDOUT_FILENO] != -1) {
		close(saved_fds[STDOUT_FILENO]);
		saved_fds[STDOUT_FILENO] = -1;
	}
	if (saved_fds[STDERR_FILENO] != -1) {
		close(saved_fds[STDERR_FILENO]);
		saved_fds[STDERR_FILENO] = -1;
	}
}

void launch_process(SimpleCommand *command, Vars *shell_vars,  pid_t pgid, bool foreground) {
	ShellInfos *shell_infos = shell(SHELL_GET);
	pid_t pid;
	if (shell_infos->interactive)
	{
		/* Put the process into the process group and give the process group
		 the terminal, if appropriate.
		 This has to be done both by the shell and in the individual
		 child processes because of potential race conditions.  */
		pid = getpid ();
		if (pgid == 0) pgid = pid;
		setpgid (pid, pgid);

		if (foreground)
			tcsetpgrp (shell_infos->shell_terminal, pgid);

		/* Set the handling for job control signals back to the default.  */
		signal_manager(SIG_EXEC);
	}
	close_all_fds();
	if (!command || !exec_simple_command(command, shell_vars)) {
		gc(GC_CLEANUP, GC_ALL);
		exit(g_exitno);
	}
}

void resolve_bin(SimpleCommand *command, Vars *shell_vars) {
	if (command && command->bin) {
		char *maybe_bin = hash_interface(HASH_FIND, command->bin, shell_vars);
		if (!is_builtin(command->bin)) {
			if (maybe_bin) {
				hash_interface(HASH_ADD_USED, command->bin, shell_vars);
				command->bin = maybe_bin;
			} else {
				maybe_bin = find_bin_location(command->bin, shell_vars->env);
				if (maybe_bin) {
					hash_interface(HASH_ADD_USED, command->bin, shell_vars);
					command->bin = maybe_bin;
				} else {
					command->bin = NULL;
				}
			}
		}
	}
}


int launch_job(Job *job, Vars *shell_vars, bool foreground) {
	ShellInfos *shell_infos = shell(SHELL_GET);
	Process *proc = job->first_process;
	int pipefd[2] = {-1 , -1};
	int saved_fds[3] = {-1, -1, -1};
	pid_t pids[1024] = {0};
	int i = 0;

	saved_fds[STDIN_FILENO] = dup(STDIN_FILENO);
	saved_fds[STDOUT_FILENO]= dup(STDOUT_FILENO);
	saved_fds[STDERR_FILENO] = dup(STDERR_FILENO);

	bool prev_pipe = false;

	while (proc) {

		if (prev_pipe) {
			secure_dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
		}
		if (proc->next) {
			prev_pipe = true;
			secure_pipe2(pipefd, O_CLOEXEC);
			secure_dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
		}
		
		if (proc->data_tag == DATA_NODE) {
			if (proc->n_data->tree_tag == TREE_COMMAND_GROUP) {
				if (proc->n_data->redirs != NULL) {
					if (apply_all_redirect(proc->n_data->redirs)) {
						g_exitno = ast_execute(proc->n_data, shell_vars, foreground);
					}
				} else {
					g_exitno = ast_execute(proc->n_data, shell_vars, foreground);
				}
			} else if (proc->n_data->tree_tag == TREE_SUBSHELL) {
				pids[i] = secure_fork();
				if (pids[i] == 0) {
					// dprintf(2, C_BRIGHT_BLUE"INFO"C_RESET": Command: "C_DARK_YELLOW"%s"C_RESET" PID | GPID: "C_YELLOW "%d | %d"C_RESET"\n", "SUBSHELL", getpid(), getpgid(getpid()));
					// close_all_fds();
					if (proc->n_data->redirs != NULL) {
						if (!apply_all_redirect(proc->n_data->redirs)) {
							gc(GC_CLEANUP, GC_SUBSHELL);
							exit(g_exitno);
						}
					}
					g_exitno = ast_execute(proc->n_data, shell_vars, foreground);
					gc(GC_CLEANUP, GC_ENV);
					gc(GC_CLEANUP, GC_SUBSHELL);
					free(((Garbage *)gc(GC_GET))[GC_GENERAL].garbage);
					exit(g_exitno);
				} else {
					proc->pid = pids[i];
					if (!job->pgid)
						job->pgid = pids[i];
					setpgid (pids[i], job->pgid);
					string_list_clear(shell_vars->local);
				}
			}
		}

		if (proc->data_tag == DATA_TOKENS) {
			SimpleCommand *command = parser_parse_current(proc->s_data, shell_vars);
			// printCommand(command);
			proc->command = command;
			if (!command && pipefd[0] == -1){
				close_all_fds();
				return false;
			}
			resolve_bin(command, shell_vars);
			//if its a not a pipeline, try to execute a builtin w/o forking
			if (pipefd[0] == -1 && builtin_executer(command, shell_vars))
				break;

			pids[i] = secure_fork();
			if (pids[i] == 0) {
				launch_process(command, shell_vars, job->pgid, foreground);
			} else {
				// Parent process
				proc->pid = pids[i];
				if (!job->pgid)
					job->pgid = pids[i];
				setpgid (pids[i], job->pgid);
				// dprintf(2, C_BRIGHT_BLUE"INFO"C_RESET": Command: "C_YELLOW"%s"C_RESET" PID | GPID: "C_YELLOW "%d | %d"C_RESET"\n", command->bin, pids[i], getpgid(pids[i]));
				string_list_clear(shell_vars->local);
			}
		}

		dup2(saved_fds[STDIN_FILENO], STDIN_FILENO);
		dup2(saved_fds[STDOUT_FILENO], STDOUT_FILENO);
		dup2(saved_fds[STDERR_FILENO], STDERR_FILENO);
		
		//increment pid only if it isnt a command group
		i += !(proc->data_tag == DATA_NODE && proc->n_data->tree_tag == TREE_COMMAND_GROUP);
		proc = proc->next;
	}

	if (!shell_infos->interactive) {
		wait_for_job(job);
	} else if (foreground) {
		put_job_in_foreground(job, false);
	} else {
		//TODO: launched
		put_job_in_background(job);
	}

	close_saved_fds(saved_fds);
	return true;
}

Job *job_init(Process *first_process) {
	Job *self = gc(GC_CALLOC, 1, sizeof(Job), GC_SUBSHELL);
	self->first_process = first_process;
	return self;
}

int exec_node(Node *node, Vars *shell_vars, bool foreground) {
	// TODO: remove ExecuterList, became useless
	const ExecuterList *list = build_executer_list(node->value.operand);
	Job *job = job_init(list->data[0]);
	launch_job(job, shell_vars, foreground);

	/*
	for (Process *p = job->first_process; p; p = p->next) {
		dprintf(2, C_BRIGHT_CYAN"%d"C_RESET": status | completed | stopped : " \
		  C_VIOLET"%d"C_RESET" | "C_LIGHT_GREEN"%d"C_RESET" | "C_LIGHT_ORANGE"%d"C_RESET"\n", \
		  p->pid, WEXITSTATUS(p->status), p->completed, p->stopped);
	} */
	
	return g_exitno;
}
