/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:22:03 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/06 17:43:18 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"
#include "signals.h"
#include "utils.h"
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
bool g_subshell = false;


void dup_input(int fd) {
	if (dup2(fd, STDIN_FILENO) == -1)
		_fatal("dup2 failed", 1);
}

void dup_output(int fd) {
	if (dup2(fd, STDOUT_FILENO) == -1)
		_fatal("dup2 failed", 1);
}

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

void execute_simple_command(CommandP *command, char *bin, Vars *shell_vars) {
	SimpleCommandP *simple_command = command->simple_command;

	if (!redirect_ios(simple_command->redir_list))
		return;
	if (!bin)
		return;
	
	close_fd_set();
	// dprintf(2, "Starting: %s with id: %d\n", bin, getpid());
	execve(bin, simple_command->word_list->data, shell_vars->env->data);
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
		ListP * const condition = if_clause->conditions->data[i];
		execute_complete_command(wrap_list(condition), shell_vars, background, background);
		if (g_exitno == 0) {
			ListP * const body = if_clause->bodies->data[i];
			execute_complete_command(wrap_list(body), shell_vars, background, background);
			return ;
		}
	}
	if (i < if_clause->bodies->size) {
		ListP *else_body = if_clause->bodies->data[i];
		execute_complete_command(wrap_list(else_body), shell_vars, background, background);
	}
}

#include "Arena.h"

void execute_while_clause(const CommandP * const command, const bool background, Vars * const shell_vars) {
	const WhileClauseP * const while_clause = command->while_clause;
	// print_list(while_clause->body);

	ArenaAllocator *arena = arena_create(1e5);

	ListP *cond = arena_dup_list(arena, while_clause->body);
	print_list(while_clause->body);
	print_list(cond);
	_fatal("salut !", 1);

	while (true) {
		ListP * const condition_save = gc_duplicate_list(while_clause->condition);
		// ListP * const condition_save = arena_dup_list(arena, while_clause->condition);
		execute_complete_command(wrap_list(condition_save), shell_vars, true, background);
		if (g_exitno != 0) break ;
		ListP * const body_save = gc_duplicate_list(while_clause->body);
		// ListP * const body_save = arena_dup_list(arena, while_clause->body);
		execute_complete_command(wrap_list(body_save), shell_vars, true, background);
	}
	arena_destroy(arena);
}

void execute_until_clause(const CommandP * const command, const bool background, Vars * const shell_vars) {
	const WhileClauseP * const while_clause = command->while_clause;

	while (true) {
		ListP * const condition_save = gc_duplicate_list(while_clause->condition);
		execute_complete_command(wrap_list(condition_save), shell_vars, true, background);
		if (g_exitno == 0) break ;
		ListP * const body_save = gc_duplicate_list(while_clause->body);
		execute_complete_command(wrap_list(body_save), shell_vars, false, background);
	}
}

void execute_case_clause(const CommandP * const command, const bool background, Vars * const shell_vars) {
	CaseClauseP * const case_clause = command->case_clause;
	da_create(tmp, StringListL, sizeof(char *), GC_SUBSHELL);
	da_push(tmp, case_clause->expression);
	case_clause->expression = do_expansions(tmp, shell_vars, O_ALLOWNULLS)->data[0];
	for (size_t i = 0; i < case_clause->patterns->size; i++) {
		case_clause->patterns->data[i] = do_expansions(case_clause->patterns->data[i], shell_vars, O_ALLOWNULLS);
	}

	int default_index = -1;
	for (size_t i = 0; i < case_clause->patterns->size; i++) {
		const StringListL * const patterns = case_clause->patterns->data[i];
		for (size_t inner_i = 0; inner_i < patterns->size; inner_i++) {
			if (!ft_strcmp("*", patterns->data[inner_i]))
				default_index = i;
			if (!ft_strcmp(patterns->data[inner_i], case_clause->expression))
				return execute_complete_command(wrap_list(case_clause->bodies->data[i]), shell_vars, true, background);
		}
	}
	if (default_index != -1)
		return execute_complete_command(wrap_list(case_clause->bodies->data[default_index]), shell_vars, true, background);
	return ;
}

void execute_for_clause(const CommandP * const command, const bool background, Vars *const shell_vars) {
	const ForClauseP * const for_clause = command->for_clause;
	const StringListL *expanded_words = do_expansions(for_clause->word_list, shell_vars, O_SPLIT | O_ALLOWNULLS);
	const StringListL * const word_list = (for_clause->in == true) ? expanded_words : shell_vars->positional;

	// print_for_clause(for_clause);
	// ListP * save = gc_duplicate_list(for_clause->body);
	// print_list(save);
	// fatal("fdp\n", 1);
	for (size_t i = (0 + !for_clause->in); word_list && i < word_list->size; i++) {
		char buffer[MAX_WORD_LEN] = {0};
		const char * const value = (for_clause->in == true) ? word_list->data[i] : get_positional_value(word_list, i); 
		const int ret = ft_snprintf(buffer, MAX_WORD_LEN, "%s=%s", for_clause->iterator, value);
		if (ret == -1)
			_fatal("snprintf: buffer overflow", 1);
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
			_fatal("snprintf: exceeded buffer capacity", 1);
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
  if (positional->size <= 1) return ;
	for (size_t i = 1; i < positional->size; i++) {
		gc(GC_FREE, positional->data[i], GC_ENV);
	}
	positional->size = 1;
}

//FIX segfault in recursive function calls
void execute_function(const CommandP * const command, const int funcNo, const bool background, Vars * const shell_vars) {
	const SimpleCommandP * const simple_command = command->simple_command;
	const StringListL * const positional_parameters = simple_command->word_list;
	StringListL * const saved_positionals = save_positionals(shell_vars);
	const FunctionP * const function_copy = gc_duplicate_function(g_funcList->data[funcNo]);

	clear_positional(shell_vars->positional);
	declare_positional(positional_parameters, shell_vars);
	// string_list_print(shell_vars->positional);
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
	if (!job->pgid && g_masterPgid != 0)
		job->pgid = g_masterPgid;
	else if (!job->pgid)
		job->pgid = pid;
	setpgid(pid, job->pgid);
}

void process_simple_command(SimpleCommandP * const simple_command, Vars *shell_vars) {
	StringListL *args = do_expansions(simple_command->word_list, shell_vars, O_SPLIT);
	StringListL *vars = do_expansions(simple_command->assign_list, shell_vars, O_NONE);
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


void restore_std_fds(int *saved_fds) {
	if (dup2(saved_fds[0], STDIN_FILENO) == -1)
		_fatal("dup2 failed: restoring stdin", 1);
	if (dup2(saved_fds[1], STDOUT_FILENO) == -1)
		_fatal("dup2 failed: restoring stdout", 1);
	if (dup2(saved_fds[2], STDERR_FILENO) == -1)
		_fatal("dup2 failed: restoring stderr", 1);
}

int execute_single_command(AndOrP *job, const bool background, Vars *shell_vars) {
	PipeLineP *process = job->pipeline;
	CommandP *command = job->pipeline->command;
	ShellInfos *shell_infos = shell(SHELL_GET);
	switch (command->type) {

		case Simple_Command: {
			//FIX: move process simple command
			process_simple_command(command->simple_command, shell_vars);
			char *bin = resolve_bine(command->simple_command, shell_vars);
			int funcNo = is_function(bin);
			if (funcNo != -1) {
				execute_function(command, funcNo, background, shell_vars);
				return NO_WAIT;
            } else if (is_builtin(bin)) {
				const bool hasRedir = (command->simple_command->redir_list->size != 0);
				int *saved_fds;

				if (hasRedir) { saved_fds = save_std_fds(); }

				if (!redirect_ios(command->simple_command->redir_list)) {
					return NO_WAIT;
				}

				execute_builtin(command->simple_command, shell_vars);

				if (hasRedir) {
					restore_std_fds(saved_fds);
					close_saved_fds(saved_fds);
				}
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
								_fatal("tcsetpgrp: failed", 1);
                        }
						signal_manager(SIG_EXEC);
					}
					execute_simple_command(command, bin, shell_vars);
					exit_clean();
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
				execute_subshell(command, background, shell_vars);
				exit_clean(); 
			} else { set_group(job, process, pid); }
			return WAIT;
		}
		case Brace_Group: { execute_brace_group(process->command, background, shell_vars); return NO_WAIT; }
		case If_Clause: { execute_if_clause(process->command, background, shell_vars); return NO_WAIT; }
		case While_Clause: { execute_while_clause(process->command, background, shell_vars); return NO_WAIT; }
		case Until_Clause: { execute_until_clause(process->command, background, shell_vars); return NO_WAIT; }
		case Case_Clause: { execute_case_clause(process->command, background, shell_vars); return NO_WAIT; }
		case For_Clause: { execute_for_clause(process->command, background, shell_vars); return NO_WAIT; }
		case Function_Definition: { register_function(process->command); return NO_WAIT; }
		default: { break; };
	}
	return ERROR;
}

int execute_pipeline(AndOrP *job, bool background, Vars *shell_vars) {
	PipeLineP *process = job->pipeline;
	ShellInfos *shell_infos = shell(SHELL_GET);

    int prev_pipe[2] = {-1, -1};
    int curr_pipe[2] = {-1, -1};

	const bool piped = (process->next != NULL);


	if (!piped) {
		int exit_info = execute_single_command(job, background, shell_vars);
		return exit_info;
	}

	while (process) {
		const bool hasNext = (process->next != NULL);

		if (hasNext && pipe(curr_pipe) == -1)
			_fatal("failed to open pipe", 1);


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
						_fatal("tcsetpgrp: failed", 1);
                }
				signal_manager(SIG_EXEC);
			}

            if (prev_pipe[0] != -1) { dup_input(prev_pipe[0]); close(prev_pipe[0]); }
            if (hasNext) { dup_output(curr_pipe[1]); close(curr_pipe[1]); }

            close(prev_pipe[1]);
            close(curr_pipe[0]);
            close(curr_pipe[1]);

			switch (process->command->type) {
				case Simple_Command: { 
					process_simple_command(process->command->simple_command, shell_vars);
					char *bin = resolve_bine(process->command->simple_command, shell_vars);
					int funcNo = is_function(bin);
					if (funcNo != -1) {
						execute_function(process->command, funcNo, background, shell_vars);
					} else if (is_builtin(bin)) {
						if (!redirect_ios(process->command->simple_command->redir_list)) {
							exit_clean();
						}
						execute_builtin(process->command->simple_command, shell_vars);
					} else {
						execute_simple_command(process->command, bin, shell_vars);
					}
					exit_clean();
					break;
				}
				case Subshell: { execute_subshell(process->command, background, shell_vars); exit_clean(); break; }
				case Brace_Group: { execute_brace_group(process->command, background, shell_vars); exit_clean(); break; }
				case If_Clause: { execute_if_clause(process->command, background, shell_vars); exit_clean(); break; }
				case While_Clause: { execute_while_clause(process->command, background, shell_vars); exit_clean(); break; }
				case Until_Clause: { execute_until_clause(process->command, background, shell_vars);  exit_clean(); break; }
				case Case_Clause: { execute_case_clause(process->command, background, shell_vars); exit_clean(); break; }
				case For_Clause: { execute_for_clause(process->command, background, shell_vars); exit_clean(); break; }
				case Function_Definition: { register_function(process->command); exit_clean(); break; }
				default: { break; }
			}
		} else { 
			if (prev_pipe[0] != -1) {
				close(prev_pipe[0]);
			}
			if (prev_pipe[1] != -1) {
				close(prev_pipe[1]);
			}
			prev_pipe[0] = curr_pipe[0];
			prev_pipe[1] = curr_pipe[1];
			set_group(job, process, pid); 
		}
		process = process->next;
	}
    close(prev_pipe[0]);
    close(prev_pipe[1]);

	return WAIT;
}

void set_exit_number(const PipeLineP * const pipeline) {
	const PipeLineP * head = pipeline;
	while (head->next) {
		head = head->next;
	}
	const int status = head->status;
	if (WIFEXITED(status)) {
		g_exitno = WEXITSTATUS(status);
	} else if (WIFSIGNALED(status)) {
		g_exitno = 128 + WTERMSIG(status);
	}
}


void execute_list(ListP *list, bool background, Vars *shell_vars) {
	AndOrP *andor_head = list->and_or;
	ShellInfos *shell_infos = shell(SHELL_GET);

	while (andor_head) {
		TokenType separator = andor_head->separator; (void)separator;
		const int wait_status = execute_pipeline(andor_head, background, shell_vars);

		if (wait_status == WAIT) {
			if (!shell_infos->interactive) {
				job_wait(andor_head);
			} else if (!background) {
				put_job_foreground(andor_head, false);
			} else if (background) {
				put_job_background(andor_head);
			}
			set_exit_number(andor_head->pipeline);
		}
		if (andor_head->pipeline->banged) {
			if		(g_exitno == 0) g_exitno = 1;
			else if (g_exitno != 0) g_exitno = 0;
		}

		if (g_exitno > 128) return ;

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
	if (subshell) g_subshell = true;
	else g_subshell = false;

	// print_complete_command(complete_command);

	while (list_head) {
		const bool background = (
			(shell_infos->interactive) && 
			( (list_head->separator == END && complete_command->separator == AMPER) ||
			(list_head->separator == AMPER) || (bg) )
		);
		if (!subshell)
			g_masterPgid = 0;

		if (shell_infos->script) {
			g_masterPgid = shell_infos->shell_pgid;
		}

		if (background == true && !subshell) {
			pid_t pid = fork();
			if (IS_CHILD(pid)) {
				g_masterPgid = getpid();
				// dprintf(2, C_BRIGHT_CYAN"[MASTER] ANNOUNCE"C_RESET": "C_MAGENTA"{ PID = %d, PGID = %d }"C_RESET"\n", getpid(), getpgid(getpid()));
				execute_list(list_head, background, shell_vars);
				exit_clean(); break;
			} else {
				setpgid(pid, pid);
				AndOrP *master = list_head->and_or;
				master->tmodes = shell_infos->shell_tmodes;
				master->subshell = subshell;
				master->id = g_jobList->size;
				master->pgid = pid;
				put_job_background(master);
			}
		} else {
			execute_list(list_head, background, shell_vars);
		}
		list_head = list_head->next;
	}
	return ;
}
