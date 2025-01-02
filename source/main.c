/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 13:44:11 by bvan-pae          #+#    #+#             */
/*   Updated: 2025/01/02 13:44:13 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_regex.h"
#include "final_parser.h"
#include "utils.h"
#include "signals.h"
#include "exec.h"
#include "libft.h"
#include "jobs.h"
#include "ft_readline.h"
#include "expansion.h"
#include "dynamic_arrays.h"

#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <sys/mman.h>
#include <signals.h>

int				g_debug = 0;
int 			g_exitno = 0;
int				g_functionCtx = 0;
IntList	*		g_fdSet = NULL;
pid_t			g_masterPgid = 0;
FunctionList *	g_funcList = NULL;
JobList *		g_jobList = NULL;


//FIX: quote bug in this case : echo "${staged:+$staged}" $ sign causes this bug surprinsigly
//FIX: readline_redisplay || ^C should process the prompt like bellow
//FIX: max 16 heredoc

static void	*read_input_prompt(char *input, Vars *const shell_vars) {
	char *PS1 = string_list_get_value(shell_vars->set, "PS1");
	// dprintf(STDERR_FILENO, "PS1 value: %s\n", PS1);

	if (PS1) {
		ShellInfos *shell_infos = shell(SHELL_GET);
		int saved_exitno = g_exitno;

		char *maybe_prompt_command;
		if ((maybe_prompt_command = get_variable_value(shell_vars, "PROMPT_COMMAND")) != NULL) {
			int saved_shellstate = shell_infos->script;

			shell_infos->script = true;
			parse_input(maybe_prompt_command, "PROMPT_COMMAND", shell_vars);
			shell_infos->script = saved_shellstate;
		}
		g_exitno = saved_exitno;
		PS1 = prompt_expansion(PS1, shell_vars);

		input = ft_readline(PS1, shell_vars);
    } else {
		input = ft_readline("42sh> ", shell_vars);
    }

	if (!input)
		return NULL;

	gc(GC_ADD, input, GC_SUBSHELL);
	return input;
}

void env_to_string_list(StringList *const env_list, const char **env){
	for (size_t i = 0; env[i]; i++)
		string_list_add_or_update(env_list, gc(GC_ADD, ft_strdup(env[i]), GC_ENV));
}

Vars *shell_vars_init(const char **env) {
	Vars *self = gc_unique(Vars, GC_ENV);

	da_create(env_list, StringList, sizeof(char *), GC_ENV);
	self->env = env_list;
	da_create(set_list, StringList, sizeof(char *), GC_ENV);
	self->set = set_list;
	da_create(local_list, StringList, sizeof(char *), GC_ENV);
	self->local = local_list;
	da_create(positional_list, StringList, sizeof(char *), GC_ENV);
	self->positional = positional_list;
	da_create(alias_list, StringList, sizeof(char *), GC_ENV);
	self->alias = alias_list;

	env_to_string_list(self->env, env);
	env_to_string_list(self->set, env);

	string_list_add_or_update(env_list, "OUTFILES='/tmp/42sh_testing/output_files");
	string_list_add_or_update(env_list, "INFILES='/tmp/42sh_testing/input_files");

	return self;
}

static char *read_input_file(const char *path){
    int fd = open(path, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
		_fatal("open: no such file", 1);

	struct stat st;
	if (fstat(fd, &st) == -1)
		_fatal("fstat: failed", 1);

    size_t file_size = st.st_size;
	if (file_size == 0) {
		close(fd);
		return NULL;
	}

    char *buffer = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buffer == MAP_FAILED) {
		close(fd);
		printf("mmap failed\n");
		return NULL;
    }

	char *file_content = gc(GC_ADD, ft_strdup(buffer), GC_SUBSHELL);
	munmap(buffer, file_size);
	close(fd);

	return file_content;
}

ShellInfos *shell(const int mode) {
	static ShellInfos *self = NULL;

	if (mode == SHELL_INIT) {
		self = gc_unique(ShellInfos, GC_ENV);

		self->shell_terminal = STDIN_FILENO;
		self->interactive = isatty(self->shell_terminal);

		if (self->interactive) {

			while (tcgetpgrp(self->shell_terminal) != (self->shell_pgid = getpgrp()))
				kill(-self->shell_pgid, SIGTTIN);

			signal (SIGINT, SIG_IGN);
			signal (SIGQUIT, SIG_IGN);
			signal (SIGTSTP, SIG_IGN);
			signal (SIGTTIN, SIG_IGN);
			signal (SIGTTOU, SIG_IGN);
			signal (SIGCHLD, SIG_IGN);
			
			self->shell_pgid = getpid();

			if (setpgid(self->shell_pgid, self->shell_pgid) == -1)
				_fatal("setpgid: couldn't set the shell in its own pgid", 1);

			if (tcsetpgrp(self->shell_terminal, self->shell_pgid) == -1)
				_fatal("tcsetpgrp: fatal", 1);
			if (tcgetattr(self->shell_terminal, &self->shell_tmodes) == -1)
				_fatal("tcgetattr: fatal", 1);
		}
	}

	return self;
}

static void update_last_executed_command(Vars *const shell_vars, const char *const input) {
	char *last_executed = ft_strjoin("_=", input);
	string_list_add_or_update(shell_vars->env, last_executed);
	string_list_add_or_update(shell_vars->set, last_executed);
	FREE_POINTERS(last_executed);
}

static void update_history_file(const HISTORY_STATE *const history, Vars *const shell_vars){
	char *histfile = get_variable_value(shell_vars, "HISTFILE");
	char *c_histfilesize = get_variable_value(shell_vars, "HISTFILESIZE");
	
	int histfilesize = -1;
	if (c_histfilesize && regex_match("[^0-9]", c_histfilesize).is_found == false){
		histfilesize = ft_atoi(c_histfilesize);
	}
	if (!histfile) 
		return;

	int history_fd = open(histfile, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (history_fd == -1)
		_fatal("open: history", 1);

	for (int i = (history->length > histfilesize && histfilesize >= 0) ? history->length - histfilesize : 0; 
		i < history->length; i++)
	{
		ft_dprintf(history_fd, "%s\n", history->entries[i]->line.data);
	}
	close(history_fd);
}


static void load_42shrc(Vars *const shell_vars) {
	get_history(shell_vars);
	shell(SHELL_GET)->script = true;
	signal_manager(SIG_SCRIPT);

	char * home = string_list_get_value(shell_vars->env, "HOME");
	char config_filename[MAX_WORD_LEN] = {0};
	ft_sprintf(config_filename, "%s/.42shrc", home);
    //FIX: file loaded even if it doens't exist

	char * const file_content = read_input_file(config_filename);
	if (file_content)
		parse_input(file_content, config_filename, shell_vars);
	shell(SHELL_GET)->script = false;
}

static void load_positional_parameters(const int ac, char ** const av, Vars * const shell_vars, ShellInfos * const shell_infos) {
	if (!shell_infos->script) {
		char buffer[] = "0=42sh";
		string_list_add_or_update(shell_vars->positional, buffer);
	} else {
		for (int i = 1; i < ac; i++){
			char buffer[MAX_WORD_LEN] = {0};
			char *positional_number = ft_itoa(i - 1);
			if (ft_snprintf(buffer, MAX_WORD_LEN, "%s=%s", positional_number, av[i]) == -1)
				_fatal("snprintf: MAX_WORD_LEN exceeded", 1);
			free(positional_number);
			string_list_add_or_update(shell_vars->positional, buffer);
		}
	}
}

static void init_globals() {
	da_create(jobListTmp, JobList, sizeof(AndOrP *), GC_ENV);
	da_create(funcListTmp, FunctionList, sizeof(FunctionP *), GC_ENV);
	da_create(fdSetTmp, IntList, sizeof(int), GC_ENV);

	g_fdSet = fdSetTmp;
	g_jobList = jobListTmp;
	g_funcList = funcListTmp;
}

#define SHELL_IS_RUNNING true
#define SCRIPT_MODE ((ac == 1) ? false : true)
#define HAS_POSITIONAL (ac > 2)

int main(const int ac, char *av[], const char *env[]) {

	init_globals();
	shell(SHELL_INIT);

	if (!env || !*env)
		_fatal("environ: environment not set", 1);

	Vars *shell_vars = shell_vars_init(env);
	ShellInfos *self = shell(SHELL_GET);

	if (SCRIPT_MODE) { self->script = true; }
	if (self->interactive && !self->script) { load_42shrc(shell_vars); }

	char *input = NULL;
	while (SHELL_IS_RUNNING) {
		if (self->interactive && !self->script) { signal_manager(SIG_PROMPT); }
		if (self->interactive && self->script) { signal_manager(SIG_SCRIPT); }
		if (!SCRIPT_MODE)
			input = read_input_prompt(input, shell_vars);
		else
			input = read_input_file(av[1]);
		load_positional_parameters(ac, av, shell_vars, self);
		if (self->interactive && !self->script) { job_notification(); }
		if (input) {
			if (self->interactive && !self->script) {
				if (history_expansion(&input) == false)
					continue;
			}
			parse_input(input, av[1], shell_vars);
			update_last_executed_command(shell_vars, input);
			gc(GC_RESET, GC_SUBSHELL);
			if (SCRIPT_MODE) { break; }
		} else {
			if (g_jobList->size) {
				ft_dprintf(STDERR_FILENO, "There are alive background jobs. Killing them.\n");
				job_killall(0);
				continue;
			} else if (self->interactive) {
				ft_dprintf(STDERR_FILENO, "exit\n");
			}
			break;
		}
	}
	if (self->interactive && !self->script) { update_history_file(history, shell_vars); }
	close_fd_set();
	gc(GC_CLEANUP, GC_ALL);
	exit(g_exitno);
}
