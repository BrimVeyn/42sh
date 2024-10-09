/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 14:56:16 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/09 10:55:21 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "ast.h"
#include "debug.h"
#include "lexer.h"
#include "parser.h"
#include "signals.h"
#include "utils.h"
#include "libft.h"
#include "colors.h"

#include <readline/history.h>
#include <signal.h>
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

typedef void (*builtin_func_t)(const SimpleCommand *, Vars *);

bool builtin_executer(const SimpleCommand *command, Vars *shell_vars) {
	if (!command->bin) 
		return false;

	static const struct {
		char *bin;
		builtin_func_t func;
	} map[] = {
		{"env",  &builtin_env}, {"set",  &builtin_set},
		/*{"echo",  &builtin_echo},*/ {"exit",  &builtin_exit},
		{"export",  &builtin_export}, {"hash",  &builtin_hash},
		{"type",  &builtin_type}, {"jobs",  &builtin_jobs},
		/*{"pwd", &builtin_pwd},*/ {"unset", &builtin_unset},
		{"fg", &builtin_fg}, 
	};

	int result_index = -1;
	for (size_t i = 0; i < sizeof(map) / sizeof(map[0]); i++) {
		if (!ft_strcmp(command->bin, map[i].bin)) {
			result_index = i;
			break;
		}
	}

	if (result_index != -1) {
		map[result_index].func(command, shell_vars);
		return true;
	}
	return false;
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

bool is_builtin(char *bin) {
	static const char *builtins[] = {
		/*"echo",*/ "cd", /*"pwd",*/ "export", "type",
		"unset", "env", "exit", "set", 
		"hash", "jobs", "fg",
	};

	for (size_t i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++) {
		if (!ft_strcmp(builtins[i], bin)) {
			return true;
		}
	}
	return false;
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

int job_is_stopped(Job *j) {
  Process *p;

  for (p = j->first_process; p; p = p->next)
    if (!p->completed && !p->stopped)
      return 0;
  return 1;
}

int job_is_completed(Job *j) {
  Process *p;

  for (p = j->first_process; p; p = p->next)
    if (!p->completed)
      return 0;
  return 1;
}

#include <errno.h>

int mark_process_status (Job *j, pid_t pid, int status) {
	Process *p;

	if (pid <= 0) {
		return -1;
	} else {
		for (; j; j = j->next) {
		/* Update the record for the process.  */
			for (p = j->first_process; p; p = p->next)
				if (p->pid == pid) {
					p->status = status;
					if (WIFEXITED(status)) {
						g_exitno = WEXITSTATUS(status);
						p->completed = 1;
						// dprintf (2, C_LIGHT_MAGENTA"EXIT"C_RESET": "C_GOLD"%d"C_RESET" status: "C_BRIGHT_BLUE"%d"C_RESET"\n", pid, WEXITSTATUS(status));
					} else if (WIFSTOPPED (status)) {
						p->stopped = 1;
						// dprintf (2, "%d: Stopped\n", pid);
					} else if (WIFSIGNALED (status)) {
						p->completed = 1;
						// dprintf (2, "%d: Terminated by signal %d.\n", (int) pid, WTERMSIG (p->status));
					}
					return 0;
				}
		}
		fprintf (stderr, C_RED"FATAL: No child process %d."C_RESET"\n", pid);
		return -1;
	}
}

void wait_for_job (Job *j) {
	int status;
	pid_t pid;

	do {
		pid = waitpid(-j->pgid, &status, WUNTRACED);
		if (pid != -1) {
			// dprintf(2, C_BRIGHT_CYAN"WAIT"C_RESET": waiting for | waited | in process: "C_MAGENTA"%d | %d | %d"C_RESET"\n", j->pgid, pid, getpid());
		} else {
			// dprintf(2, C_BRIGHT_CYAN"WAIT"C_RESET": waiting for | waited | in process: "C_RED"%d | %d | %d"C_RESET"\n", j->pgid, pid,  getpid());
		}
	} while (!mark_process_status(j, pid, status)
	&& !job_is_stopped(j)
	&& !job_is_completed(j));
}

void	job_list_addback(Job **lst, Job *new_value)
{
	Job	*temp;

	if (*lst == NULL)
		*lst = new_value;
	else
	{
		temp = *lst;
		while (temp->next)
			temp = temp->next;
		temp->next = new_value;
	}
}

void put_job_in_background (Job *j, int cont) {
	job_list_addback(&job_list, j);
	//Send cont signal if necessary (wake up the job)
	if (cont)
		if (kill (-j->pgid, SIGCONT) < 0)
			perror ("kill (SIGCONT)");
}

void update_status(void) {
	int status;
	pid_t pid;

	do {
		pid = waitpid (WAIT_ANY, &status, WUNTRACED|WNOHANG);
		dprintf(2, "Waited for: %d\n", pid);
	} while (!mark_process_status (job_list, pid, status));
}

void format_job_info (Job *j, const char *status) {
  dprintf(2, "%ld (%s)\n", (long)j->pgid, status);
}

void do_job_notification(void) {
	update_status();

	Job *jlast = NULL;
	Job *jnext = NULL;
	for (Job *j = job_list; j; j = j->next) {
		jnext = j->next;

		if (job_is_completed(j)) {
			format_job_info(j, "completed");
			if (jlast) {
				jlast->next = jnext;
			} else {
				job_list = jnext;
			}
		} else if (job_is_stopped(j) && !j->notified) {
			format_job_info(j, "stopped");
			j->notified = true;
			jlast = j;
		} else {
			jlast = j;
		}
	}
}

void put_job_in_foreground (Job *j, int cont) {
	ShellInfos *self = shell(SHELL_GET);
	/* Put the job into the foreground.  */
	tcsetpgrp (self->shell_terminal, j->pgid);
	/* Send the job a continue signal, if necessary.  */
	if (cont) {
		tcsetattr (self->shell_terminal, TCSADRAIN, &j->tmodes);
		if (kill (- j->pgid, SIGCONT) < 0)
			perror ("kill (SIGCONT)");
	}
	/* Wait for it to report.  */
	wait_for_job (j);
	/* Put the self->shell back in the foreground.  */
	tcsetpgrp (self->shell_terminal, self->shell_pgid);
	/* Restore the self->shell’s terminal modes.  */
	tcgetattr (self->shell_terminal, &j->tmodes);
	tcsetattr (self->shell_terminal, TCSADRAIN, &self->shell_tmodes);
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
				launch_process(command, shell_vars, job->pgid, true);
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
		put_job_in_background(job, false);
		format_job_info (job, "launched");
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
