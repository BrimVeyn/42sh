/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_executer.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 11:53:01 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/18 17:35:47 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"
#include "parser.h"
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

int is_function(char *func_name);

char  *resolve_bine(SimpleCommandP *command, Vars *shell_vars) {
	if (!command || !command->word_list->data[0]) {
		return NULL;
	}
	if (is_function(command->word_list->data[0]) != -1)
		return command->word_list->data[0];
	char *maybe_bin = hash_interface(HASH_FIND, command->word_list->data[0], shell_vars);
	if (is_builtin(command->word_list->data[0])) {
		return command->word_list->data[0];
	}
	if (maybe_bin) {
		hash_interface(HASH_ADD_USED, command->word_list->data[0], shell_vars);
		return maybe_bin;
	} else {
		bool absolute = false;
		maybe_bin = find_bin_location(command->word_list->data[0], shell_vars->env, &absolute);
		if (maybe_bin) {
			if (absolute == false)
				hash_interface(HASH_ADD_USED, command->word_list->data[0], shell_vars);
			return maybe_bin;
		} else {
			return NULL;
		}
	}
}

#define MAX_FD 1024
#include <sys/stat.h>

bool check_filepath(char *file_path, TokenType mode) {
	struct stat file_stat;
	if (stat(file_path, &file_stat) != -1) {

		if (S_ISDIR(file_stat.st_mode)) {
			error("42sh: %s: Is a directory", 1);
			return (false);
		}
		if (file_stat.st_mode & S_IXUSR) {
			return file_path;
		}
		else {
			error("42sh: %s: Permission Denied", 1);
			return (false);
		}
	} else {
		if (mode == LESS) {
			error("42sh: %s: No such file or directory", 1);
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
			error("42sh: %d: Bad file descriptor", 1);
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
	if (!redirect_ios(simple_command->redir_list)) {
		return ;
	}
	char *path = resolve_bine(simple_command, shell_vars);
	if (!path) {
		return ;
	}

	dprintf(2, C_RED"-------------------------------------------"C_RESET"\n");
	dprintf(2, "  Bin: %s\n", path);
	print_simple_command(simple_command);
	dprintf(2, C_RED"-------------------------------------------"C_RESET"\n");

	execve(path, simple_command->word_list->data, shell_vars->env->data);
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

void execute_case_clause(CommandP *command, Vars *shell_vars) {
	CaseClauseP *case_clause = command->case_clause;
	int default_index = -1;
	for (size_t i = 0; i < case_clause->patterns->size; i++) {
		const StringListL *condition = case_clause->patterns->data[i];
		for (size_t inner_i = 0; inner_i < condition->size; inner_i++) {
			if (!ft_strcmp("*", condition->data[inner_i]))
				default_index = i;
			if (!ft_strcmp(condition->data[inner_i], case_clause->expression))
				return execute_list(case_clause->bodies->data[i], false, shell_vars);
		}
	}
	if (default_index != -1)
		return execute_list(case_clause->bodies->data[default_index], false, shell_vars);
	return ;
}

void execute_for_clause(CommandP *command, Vars *shell_vars) {
	ForClauseP *for_clause = command->for_clause;
	(void) for_clause; (void)shell_vars;
	return ;
}

#define NO_WAIT 0
#define WAIT 1

int is_function(char *func_name) {
	for (size_t i = 0; i < FuncList->size; i++) {
		if (!ft_strcmp(FuncList->data[i]->function_name, func_name)) {
			return i;
		}
	}
	return -1;
}

void gc_move_function(FunctionP *func);

void register_function(CommandP *command) {
	FunctionP *func = command->function_definition;
	for (size_t i = 0; i < FuncList->size; i++) {
		if (!ft_strcmp(func->function_name, FuncList->data[i]->function_name)) {
			da_erase_index(FuncList, i);
		}
	}
	gc_move_function(func);
	dprintf(2, "FUNCTION: %s\n", func->function_name);
	da_push(FuncList, func);
}

int execute_single_command(AndOrP *job, bool background, Vars *shell_vars) {
	(void)background;
	PipeLineP *process = job->pipeline;
	CommandP *command = job->pipeline->command;
	switch (command->type) {
		case Simple_Command: {
			char *path = resolve_bine(command->simple_command, shell_vars);
			int funcNo = is_function(path);
			if (funcNo != -1) {
				//FIX: reset pids
				execute_brace_group(FuncList->data[funcNo]->function_body, shell_vars);
				return NO_WAIT;
            }
			if (is_builtin(path)) {
				execute_builtin(command->simple_command, shell_vars);
				return NO_WAIT;
			} else {
				pid_t pid = fork();
				if (IS_CHILD(pid)) {
					close_all_fds();
					execute_simple_command(command, NULL, shell_vars);
					gc(GC_CLEANUP, GC_ALL);
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
		case Case_Clause: { execute_case_clause(process->command, shell_vars); return NO_WAIT; }
		case For_Clause: { execute_for_clause(process->command, shell_vars); return NO_WAIT; }
		case Function_Definition: { register_function(process->command); return NO_WAIT; }
		default: { break; };
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

	if (!piped) {
		return execute_single_command(job, background, shell_vars);
	}

	while (process) {
		const bool hasPipe = (process->next != NULL);

		// dprintf(2, "hasPipe: %s | hadPipe: %s\n", boolStr(hasPipe), boolStr(hadPipe));

		if (hadPipe) dup_input_and_close(pipefd[0]);
		if (hasPipe) create_pipe(&hadPipe, pipefd);

		pid_t pid = fork();

		if (IS_CHILD(pid)) {
			close_all_fds();

			switch (process->command->type) {
				case Simple_Command: { execute_simple_command(process->command, pipefd, shell_vars); exit(g_exitno); break; }
				case Subshell: { execute_subshell(process->command, shell_vars); exit(g_exitno); break; }
				case Brace_Group: { execute_brace_group(process->command, shell_vars); exit(g_exitno); break; }
				case If_Clause: { execute_if_clause(process->command, shell_vars); exit(g_exitno); break; }
				case While_Clause: { execute_while_clause(process->command, shell_vars); exit(g_exitno); break; }
				case Case_Clause: { execute_case_clause(process->command, shell_vars); exit(g_exitno); break; }
				case For_Clause: { execute_for_clause(process->command, shell_vars); exit(g_exitno); break; }
				case Function_Definition: { register_function(process->command); exit(g_exitno); break; }
				default: { break; }
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
		const TokenType separator = andor_head->separator; (void)separator;
		const int wait_status = execute_pipeline(andor_head, background, shell_vars);

		if (wait_status == WAIT) {
			if (!shell_infos->interactive)
				job_wait(andor_head);
			else if (background)
				put_job_background(list->and_or);
			else
				put_job_foreground(list->and_or, false);
		}

		const bool skip = (
			(separator == AND_IF && g_exitno != 0) 
			||
			(separator == OR_IF && g_exitno == 0)
		);

		andor_head = andor_head->next;
		const bool to_find = (separator == AND_IF) ? OR_IF : AND_IF; 
		bool found = false;

		while (!found && skip && andor_head) {
			if (andor_head->separator == to_find || andor_head->separator == END)
				found = true;
			andor_head = andor_head->next;
		}
	}
}


void execute_complete_command(CompleteCommandP *complete_command, Vars *shell_vars) {
	ListP *list_head = complete_command->list;
	ShellInfos *shell_infos = shell(SHELL_GET);

	const bool background = (shell_infos->interactive && complete_command->separator == AMPER);
	dprintf(2, "BACKGROUND : %s\n", boolStr(background));

	if (background) {
		pid_t pid = fork();
		if (IS_CHILD(pid)) {
			while (list_head) {
				execute_list(list_head, false, shell_vars);
				list_head = list_head->next;
			}
		} else {
			return ;
		}
	}

	while (list_head) {
		execute_list(list_head, false, shell_vars);
		list_head = list_head->next;
	}

	dprintf(2, C_LIGHT_PINK"COMPLETE COMMAND EXECUTED"C_RESET"\n");
	return ;
}
