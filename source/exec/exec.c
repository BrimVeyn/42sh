/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 11:38:04 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/28 16:37:45 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"
#include "lexer.h"
#include "parser.h"
#include "signals.h"
#include "utils.h"
#include "colors.h"
#include "libft.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

pid_t g_masterPgid = 0;

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

int is_function(const char * const func_name);

char  *resolve_bine(SimpleCommandP * const command, Vars * const shell_vars) {
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
	// print_redir_list(redir_list);

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

void execute_simple_command(CommandP *command, char *bin, int *pipefd, Vars *shell_vars) {
	SimpleCommandP *simple_command = command->simple_command;
	if (!redirect_ios(simple_command->redir_list)) {
		return ;
	}
	if (!bin) {
		return ;
	}
	execve(bin, simple_command->word_list->data, shell_vars->env->data);
	if (pipefd) {
		close(pipefd[0]);
		close(pipefd[1]);
	}
}


void execute_list(ListP *list, const bool background, Vars * const shell_vars);

CompleteCommandP *wrap_list(ListP * const list) {
	CompleteCommandP * const complete_command = gc_unique(CompleteCommandP, GC_SUBSHELL);
	complete_command->list = list;
	return complete_command;
}

void execute_subshell(const CommandP * const command, const bool background, Vars * const shell_vars) {
	ListP * const subshell = command->subshell;
	if (!redirect_ios(command->redir_list)) {
		return ;
	}
	execute_complete_command(wrap_list(subshell), shell_vars, true, background);
}

void execute_brace_group(const CommandP * const command, const bool background, Vars * const shell_vars) {
	ListP * const command_group = command->brace_group;
	if (!redirect_ios(command->redir_list)) {
		return ;
	}
	execute_complete_command(wrap_list(command_group), shell_vars, false, background);
}

void execute_if_clause(const CommandP * const command, const bool background, Vars * const shell_vars) {
	const IFClauseP * const if_clause = command->if_clause;

	size_t i = 0;
	for (; i < if_clause->conditions->size; i++) {
		ListP *condition = if_clause->conditions->data[i];
		execute_complete_command(wrap_list(condition), shell_vars, background, background);
		if (g_exitno == 0) {
			ListP *body = if_clause->bodies->data[i];
			execute_complete_command(wrap_list(body), shell_vars, background, background);
			return ;
		}
	}
	if (i < if_clause->bodies->size) {
		ListP *else_body = if_clause->bodies->data[i];
		execute_complete_command(wrap_list(else_body), shell_vars, background, background);
	}
}

void execute_while_clause(const CommandP * const command, const bool background, Vars * const shell_vars) {
	const WhileClauseP * const while_clause = command->while_clause;

	while (true) {
		ListP * const condition_save = gc_duplicate_list(while_clause->condition);
		execute_complete_command(wrap_list(condition_save), shell_vars, true, background);
		if (g_exitno != 0) break ;
		ListP * const body_save = gc_duplicate_list(while_clause->body);
		execute_complete_command(wrap_list(body_save), shell_vars, false, background);
		if (g_exitno != 0) break ;
	}
}

void execute_until_clause(const CommandP * const command, const bool background, Vars * const shell_vars) {
	const WhileClauseP * const while_clause = command->while_clause;

	while (true) {
		ListP * const body_save = gc_duplicate_list(while_clause->body);
		execute_complete_command(wrap_list(body_save), shell_vars, false, background);
		if (g_exitno != 0) break ;
		ListP * const condition_save = gc_duplicate_list(while_clause->condition);
		execute_complete_command(wrap_list(condition_save), shell_vars, true, background);
		if (g_exitno != 0) break ;
	}
}

void execute_case_clause(const CommandP * const command, const bool background, Vars * const shell_vars) {
	const CaseClauseP * const case_clause = command->case_clause;
	int default_index = -1;
	for (size_t i = 0; i < case_clause->patterns->size; i++) {
		const StringListL *condition = case_clause->patterns->data[i];
		for (size_t inner_i = 0; inner_i < condition->size; inner_i++) {
			if (!ft_strcmp("*", condition->data[inner_i]))
				default_index = i;
			if (!ft_strcmp(condition->data[inner_i], case_clause->expression))
				return execute_complete_command(wrap_list(case_clause->bodies->data[i]), shell_vars, true, background);
		}
	}
	if (default_index != -1)
		return execute_complete_command(wrap_list(case_clause->bodies->data[default_index]), shell_vars, true, background);
	return ;
}

void execute_for_clause(const CommandP * const command, const bool background, Vars *const shell_vars) {
	const ForClauseP * const for_clause = command->for_clause;
	const StringListL *expanded_words = do_expansions(for_clause->word_list, shell_vars, true);
	const StringListL * const word_list = (for_clause->in == true) ? expanded_words : shell_vars->positional;
	for (size_t i = (0 + !for_clause->in); word_list && i < word_list->size; i++) {
		char buffer[MAX_WORD_LEN] = {0};
		const char * const value = (for_clause->in == true) ? word_list->data[i] : get_positional_value(word_list, i); 
		const int ret = ft_snprintf(buffer, MAX_WORD_LEN, "%s=%s", for_clause->iterator, value);
		if (ret == -1)
			fatal("snprintf: buffer overflow", 255);
		string_list_add_or_update(shell_vars->set, buffer);
		ListP * const body_save = gc_duplicate_list(for_clause->body);
		execute_complete_command(wrap_list(body_save), shell_vars, background, background);
	}
	return ;
}

void declare_positional(const StringListL * const positional_parameters, const Vars * const shell_vars) {
	for (size_t i = 1; i < positional_parameters->size; i++) {
		char buffer[MAX_WORD_LEN] = {0};
		const int ret = ft_snprintf(buffer, MAX_WORD_LEN, "%ld=%s", i, positional_parameters->data[i]);
		if (ret == -1)
			fatal("snprintf: exceeded buffer capacity", 255);
		// dprintf(2, "BUFFER: %s\n", buffer);
		string_list_add_or_update(shell_vars->positional, buffer);
	}
}

StringListL *save_positionals(const Vars * const shell_vars) {
	da_create(saved_positionals, StringListL, sizeof(char *), GC_ENV);
	for (size_t i = 0; i < shell_vars->positional->size; i++) {
		char * const copy = gc(GC_ADD, ft_strdup(shell_vars->positional->data[i]), GC_ENV);
		da_push(saved_positionals, copy);
	}
	return saved_positionals;
}

void clear_positional(StringListL * const positional) {
	for (size_t i = 1; i < positional->size; i++) {
		gc(GC_FREE, positional->data[i], GC_ENV);
	}
	positional->size = 1;
}

void execute_function(const CommandP * const command, const int funcNo, const bool background, Vars * const shell_vars) {
	const SimpleCommandP * const simple_command = command->simple_command;
	const StringListL * const positional_parameters = simple_command->word_list;
	StringListL * const saved_positionals = save_positionals(shell_vars);
	const FunctionP * const function_copy = gc_duplicate_function(g_funcList->data[funcNo]);

	clear_positional(shell_vars->positional);
	declare_positional(positional_parameters, shell_vars);
	execute_brace_group(function_copy->function_body, background, shell_vars);
	string_list_clear(shell_vars->positional);

	shell_vars->positional = saved_positionals;
}


int is_function(const char * const func_name) {
	for (size_t i = 0; i < g_funcList->size; i++) {
		if (!ft_strcmp(g_funcList->data[i]->function_name, func_name))
			return i;
	}
	return -1;
}

void register_function(const CommandP * const command) {
	FunctionP * const func = command->function_definition;
	for (size_t i = 0; i < g_funcList->size; i++) {
		if (!ft_strcmp(func->function_name, g_funcList->data[i]->function_name)) {
			da_erase_index(g_funcList, i);
		}
	}
	gc_move_function(func);
	da_push(g_funcList, func);
}


void set_group(AndOrP * const job, PipeLineP * const process, const pid_t pid) {
	process->pid = pid;
	if (!job->pgid)
		job->pgid = pid;
	setpgid(pid, job->pgid);
}

void process_simple_command(SimpleCommandP * const simple_command, Vars *shell_vars) {
	StringListL *args = do_expansions(simple_command->word_list, shell_vars, true);
	StringListL *vars = do_expansions(simple_command->assign_list, shell_vars, false);
	simple_command->word_list = args;
	simple_command->assign_list = vars;
	if (!args->data[0])
		add_vars_to_set(shell_vars, vars);
	else
		add_vars_to_local(shell_vars->local, vars);
}

void redirect_input_to_null() {
	int devnull = open("/dev/null", O_RDONLY);
	dup2(devnull, STDIN_FILENO);
	close(devnull);
}

int execute_single_command(AndOrP *job, const bool background, Vars *shell_vars) {
	PipeLineP *process = job->pipeline;
	CommandP *command = job->pipeline->command;
	ShellInfos *shell_infos = shell(SHELL_GET);
	switch (command->type) {
		case Simple_Command: {
			process_simple_command(command->simple_command, shell_vars);
			char *bin = resolve_bine(command->simple_command, shell_vars);
			int funcNo = is_function(bin);
			if (funcNo != -1) {
				execute_function(command, funcNo, background, shell_vars);
				return NO_WAIT;
            } else if (is_builtin(bin)) {
				execute_builtin(command->simple_command, shell_vars);
				return NO_WAIT;
			} else {
				pid_t pid = fork();
				if (IS_CHILD(pid)) {

					if (shell_infos->interactive && !shell_infos->script)
					{
						pid_t pid = getpid();
						if (job->pgid == 0) 
							job->pgid = pid;
						pid_t pgid = (g_masterPgid != 0) ? g_masterPgid : job->pgid;
						setpgid(pid, pgid);

						if (!background) {
							if (tcsetpgrp(shell_infos->shell_terminal, pgid) == -1)
								fatal("tcestgrp: failed", 255);
                        }
						signal_manager(SIG_EXEC);
					}
					// if (background && shell_infos->script) { redirect_input_to_null(); }
					// setpgid(pid, g_masterPgid);
			// dprintf(2, C_DARK_CYAN"[SLAVE] ANNOUNCE"C_RESET": "C_MAGENTA"{ PID = %d, PGID = %d }"C_RESET"\n", getpid(), getpgid(getpid()));
					close_all_fds();
					execute_simple_command(command, bin, NULL, shell_vars);
					gc(GC_CLEANUP, GC_ALL);
					exit(g_exitno);
				} else { set_group(job, process, pid); }
				return WAIT;
			}
		}
		case Subshell: {
			if (background) {
				execute_subshell(command, background, shell_vars);
				return NO_WAIT;
			}
			pid_t pid = fork();
			if (IS_CHILD(pid)) {
				close_all_fds();
				// if (background && shell_infos->script) { redirect_input_to_null(); }
				execute_subshell(command, background, shell_vars);
				gc(GC_CLEANUP, GC_ALL);
				exit(g_exitno);
			} else { set_group(job, process, pid); }
			return WAIT;
		}
		case Brace_Group: { execute_brace_group(process->command, background, shell_vars); return NO_WAIT; }
		case If_Clause: { execute_if_clause(process->command, background, shell_vars); return NO_WAIT; }
		case While_Clause: { execute_while_clause(process->command, background, shell_vars);  return NO_WAIT; }
		case Until_Clause: { execute_until_clause(process->command, background, shell_vars);  return NO_WAIT; }
		case Case_Clause: { execute_case_clause(process->command, background, shell_vars); return NO_WAIT; }
		case For_Clause: { execute_for_clause(process->command, background, shell_vars); return NO_WAIT; }
		case Function_Definition: { register_function(process->command); return NO_WAIT; }
		default: { break; };
	}
	return ERROR;
}

int execute_pipeline(AndOrP *job, bool background, Vars *shell_vars) {
	PipeLineP *process = job->pipeline;
	int saved_fds[] = {
		[STDIN_FILENO] = dup(STDIN_FILENO),
		[STDOUT_FILENO] = dup(STDOUT_FILENO),
		[STDERR_FILENO] = dup(STDERR_FILENO),
	};

	ShellInfos *shell_infos = shell(SHELL_GET);
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
			if (shell_infos->interactive && !shell_infos->script)
			{
				pid_t pid = getpid();
				if (job->pgid == 0) 
					job->pgid = pid;
				pid_t pgid = (g_masterPgid != 0) ? g_masterPgid : job->pgid;
				setpgid(pid, pgid);

				if (!background)
					if (tcsetpgrp(shell_infos->shell_terminal, pgid) == -1)
						perror("setpgrp");

				if (!shell_infos->script) signal_manager(SIG_EXEC);
			}
			// if (background && shell_infos->script) { redirect_input_to_null(); }
			close_all_fds();

			switch (process->command->type) {
				case Simple_Command: { 
					process_simple_command(process->command->simple_command, shell_vars);
					char *bin = resolve_bine(process->command->simple_command, shell_vars);
					int funcNo = is_function(bin);
					if (funcNo != -1) {
						execute_function(process->command, funcNo, background, shell_vars);
					} else if (is_builtin(bin)) {
						execute_builtin(process->command->simple_command, shell_vars);
					} else {
						execute_simple_command(process->command, bin, pipefd, shell_vars);
					}
					gc(GC_CLEANUP, GC_ALL);
					exit(g_exitno);
					break;
				}
				case Subshell: { execute_subshell(process->command, background, shell_vars); gc(GC_CLEANUP, GC_ALL); exit(g_exitno); break; }
				case Brace_Group: { execute_brace_group(process->command, background, shell_vars); gc(GC_CLEANUP, GC_ALL); exit(g_exitno); break; }
				case If_Clause: { execute_if_clause(process->command, background, shell_vars); gc(GC_CLEANUP, GC_ALL); exit(g_exitno); break; }
				case While_Clause: { execute_while_clause(process->command, background, shell_vars); gc(GC_CLEANUP, GC_ALL); exit(g_exitno); break; }
				case Until_Clause: { execute_until_clause(process->command, background, shell_vars);  gc(GC_CLEANUP, GC_ALL); exit(g_exitno); break; }
				case Case_Clause: { execute_case_clause(process->command, background, shell_vars); gc(GC_CLEANUP, GC_ALL); exit(g_exitno); break; }
				case For_Clause: { execute_for_clause(process->command, background, shell_vars); gc(GC_CLEANUP, GC_ALL); exit(g_exitno); break; }
				case Function_Definition: { register_function(process->command); gc(GC_CLEANUP, GC_ALL); exit(g_exitno); break; }
				default: { break; }
			}
		} else { set_group(job, process, pid); }
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
		TokenType separator = andor_head->separator; (void)separator;
		const int wait_status = execute_pipeline(andor_head, background, shell_vars);

		if (wait_status == WAIT) {
			if (!shell_infos->interactive || background)
				job_wait_2(andor_head); //WAIT_ANY
			else { //interactive
				put_job_foreground(andor_head, false);
			}
		}
		if (WIFSIGNALED(g_exitno)) return ;
		bool skip = (
			(separator == AND_IF && g_exitno != 0) ||
			(separator == OR_IF && g_exitno == 0)
		);
		if (skip) {
			do {
				separator = andor_head->separator;
				skip = ((separator == AND_IF && g_exitno) || (separator == OR_IF && !g_exitno));
			} while (skip && (andor_head = andor_head->next));
			andor_head = andor_head->next;
		} else {
			andor_head = andor_head->next;
		}
	}
}

void execute_complete_command(CompleteCommandP *complete_command, Vars *shell_vars, bool subshell, bool bg) {
	ListP *list_head = complete_command->list;
	ShellInfos *shell_infos = shell(SHELL_GET);

	while (list_head) {
		const bool background = (
			(shell_infos->interactive) && 
			( (list_head->separator == END && complete_command->separator == AMPER) ||
			(list_head->separator == AMPER) || (bg) )
		);
		// dprintf(2, "Background ? %s\n", boolStr(background));
		if (!subshell)
			g_masterPgid = 0;

		if (background == true && !subshell) {
			pid_t pid = fork();
			if (IS_CHILD(pid)) {
				g_masterPgid = getpid();
				// dprintf(2, C_BRIGHT_CYAN"[MASTER] ANNOUNCE"C_RESET": "C_MAGENTA"{ PID = %d, PGID = %d }"C_RESET"\n", getpid(), getpgid(getpid()));
				execute_list(list_head, background, shell_vars);
				if (WIFSIGNALED(g_exitno)) return ;
				close_all_fds();
				gc(GC_CLEANUP, GC_ALL);
				exit(g_exitno);
			} else {
				setpgid(pid, pid);
				AndOrP *master = list_head->and_or;
				master->tmodes = shell_infos->shell_tmodes;
				master->subshell = subshell;
				master->id = g_jobList->size;
				master->pgid = pid;
				put_job_background(master, true);
			}
		} else {
			// signal_manager(SIG_EXEC);
			execute_list(list_head, background, shell_vars);
			if (WIFSIGNALED(g_exitno)) return ;
		}
		list_head = list_head->next;
	}
	return ;
}
