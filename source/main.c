#include "ft_regex.h"
#include "lexer/final_parser.h"
#include "parser.h"
#include "utils.h"
#include "signals.h"
#include "debug.h"
#include "lexer.h"
#include "exec.h"
#include "libft.h"
#include "ft_readline.h"

#include <linux/limits.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <sys/mman.h>
#include <signals.h>

int g_debug = 0;
int g_exitno = 0;
int g_signal = 0;

FunctionList *g_funcList = NULL;
JobListe *g_jobList = NULL;

//FIX: tilde expansion

void	*read_input_prompt(char *input, Vars *shell_vars) {
	char *prompt = string_list_get_value(shell_vars->set, "PS1");
	if (prompt)
		input = ft_readline(prompt, shell_vars);
	else
		input = ft_readline("42sh> ", shell_vars);
	if (!input) {
		return NULL;
	}
	gc(GC_ADD, input, GC_SUBSHELL);
	return input;
}

void env_to_string_list(StringList *env_list, const char **env){
	for (size_t i = 0; env[i]; i++)
		string_list_add_or_update(env_list, gc(GC_ADD, ft_strdup(env[i]), GC_ENV));
}

Vars *shell_vars_init(const char **env) {
	Vars *self = gc(GC_ADD, ft_calloc(1, sizeof(Vars)), GC_ENV);

	da_create(env_list, StringList, sizeof(char *), GC_ENV);
	self->env = env_list;
	da_create(set_list, StringList, sizeof(char *), GC_ENV);
	self->set = set_list;
	da_create(local_list, StringList, sizeof(char *), GC_ENV);
	self->local = local_list;
	env_to_string_list(self->env, env);
	env_to_string_list(self->set, env);
	return self;
}

char *get_event_number(char *string){
	char *number = ft_calloc(10, sizeof(char));
	int i = 0;
	while (string[i] && is_number(&string[i])){
		number[i] = string[i];
		i++;
		if (i >= 10)
			break;
	}
	if (i <= 0 || i >= 10){
		return NULL;
	}
	return number;
}

char *get_history_index(char *string, int index){
	int cpt = 0;
	int old_end = 0;
	if (index < 0){
		return NULL;
	}

	for (int i = 0; string[i]; i++){
		if (string[i] == '\n'){
			cpt++;
			old_end = i + 1;
		}
		if (cpt == index){
			return ft_substr(string, old_end, i - old_end - 1);
		}
	}
	return NULL;
}


char *read_input_file(char *path){
    int fd = open(path, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("Can't open file");
        exit(EXIT_FAILURE);
    }

	struct stat st;
	if (fstat(fd, &st) == -1){
        perror("Can't get file's stats");
        exit(EXIT_FAILURE);
	}
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

	char *file_content = ft_strdup(buffer);
	gc(GC_ADD, file_content, GC_SUBSHELL);
	munmap(buffer, file_size);
	close(fd);
	return file_content;
}

ShellInfos *shell(int mode) {
	static ShellInfos *self = NULL;

	if (mode == SHELL_INIT) {
		self = gc(GC_CALLOC, 1, sizeof(ShellInfos), GC_ENV);
		self->shell_terminal = STDIN_FILENO;
		self->interactive = isatty(self->shell_terminal);
		if (self->interactive) {
			while (tcgetpgrp(self->shell_terminal) != (self->shell_pgid = getpgrp()))
				kill(- self->shell_pgid, SIGTTIN);
			tcsetpgrp(self->shell_terminal, self->shell_pgid);
			tcgetattr(self->shell_terminal, &self->shell_tmodes);
		}
	} else {
		return self;
	}
	return NULL;
}

void update_last_executed_command(Vars *shell_vars, char *input) {
	char *last_executed = ft_strjoin("_=", input);
	string_list_add_or_update(shell_vars->env, last_executed);
	string_list_add_or_update(shell_vars->set, last_executed);
	FREE_POINTERS(last_executed);
}

void update_history_file(HISTORY_STATE *history, Vars *shell_vars){
	char *histfile = get_variable_in_bi(shell_vars, "HISTFILE");
	char *c_histfilesize = get_variable_in_bi(shell_vars, "HISTFILESIZE");
	// printf("%s\n", c_histfilesize);
	
	int histfilesize = -1;
	if (c_histfilesize && regex_match("[^0-9]", c_histfilesize).is_found == false){
		histfilesize = ft_atoi(c_histfilesize);
	}

	int history_fd = open(histfile, O_CREAT | O_TRUNC | O_WRONLY);
	// char *home = getenv("HOME");
	// char history_filename[1024] = {0};
	// ft_sprintf(history_filename, "%s/.42sh_history", home);
	// int history_fd = open(history_filename, O_TRUNC | O_RDWR | O_CREAT, 0644);
	// printf("history_fd: %d\n", history_fd);

	if (history_fd != -1){
		for (int i = (history->length > histfilesize && histfilesize >= 0) ? history->length - histfilesize : 0; 
		i < history->length; i++){
			ft_dprintf(history_fd, "%s\n", history->entries[i]->line.data);
		}
		close(history_fd);
	}
}

#define SHELL_IS_RUNNING true
#define SCRIPT_MODE ((ac == 1) ? false : true)

void load_42shrc(Vars *shell_vars) {
	get_history(shell_vars);
	signal_manager(SIG_PROMPT);

	char *home = string_list_get_value(shell_vars->env, "HOME");
	char config_filename[1024] = {0};
	ft_sprintf(config_filename, "%s/.42shrc", home);

	char *file_content = read_input_file(config_filename);
	//FIX: uncomment this line
	(void)file_content;
	// parse_input(file_content, config_filename, shell_vars);
}

int main(const int ac, char *av[], const char *env[]) {
	
	shell(SHELL_INIT);
	da_create(jobListTmp, JobListe, sizeof(AndOrP *), GC_ENV);
	da_create(funcListTmp, FunctionList, sizeof(FunctionP *), GC_ENV);
	g_jobList = jobListTmp;
	g_funcList = funcListTmp;

	Vars *shell_vars = shell_vars_init(env);
	ShellInfos *self = shell(SHELL_GET);

	if (SCRIPT_MODE) { self->interactive = false; }
	if (self->interactive) { load_42shrc(shell_vars); }

	char *input = NULL;
	while (SHELL_IS_RUNNING) {
		if (self->interactive) { signal_manager(SIG_PROMPT); }
		if (!SCRIPT_MODE) {
			input = read_input_prompt(input, shell_vars);
		} else {
			input = read_input_file(av[1]);
			if (ac > 2) {
				da_create(positional_args, StringList, sizeof(char *), GC_SUBSHELL);
				for (int i = 2; i < ac; i++){
					da_push(positional_args, av[i]);
				}
			}
		}
		if (self->interactive) { job_notification(); }
		if (input) {
			if (self->interactive) {
				if (history_expansion(&input) == false)
					continue;
				if (*input)
					add_history(input, shell_vars);
			}
			parse_input(input, av[1], shell_vars);
			update_last_executed_command(shell_vars, input);
			gc(GC_RESET, GC_SUBSHELL);
			if (SCRIPT_MODE) { break; }
		} else {
			if (g_jobList->size) {
				dprintf(2, "There are running jobs. Killing them.\n");
				job_killall();
				continue;
			}
			break;
		}
	}
	update_history_file(history, shell_vars);
	gc(GC_CLEANUP, GC_ALL);
	close_all_fds();
	return (EXIT_SUCCESS);
}
