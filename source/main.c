#include "parser.h"
#include "utils.h"
#include "signals.h"
#include "debug.h"
#include "lexer.h"
#include "ast.h"
#include "exec.h"
#include "libft.h"
#include "ft_readline.h"

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
JobList *job_list = NULL;

void	*init_prompt_and_signals(char *input, bool shell_is_interactive) {
	if (shell_is_interactive)
		signal_manager(SIG_PROMPT);
	
	input = ft_readline("42sh > ");
	if (!input) {
		return NULL;
	}
	return input;
}

void env_to_string_list(StringList *env_list, const char **env){
	for (uint16_t i = 0; env[i]; i++)
		string_list_add_or_update(env_list, gc(GC_ADD, ft_strdup(env[i]), GC_ENV));
}

Vars *vars_init(const char **env) {
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

int main(const int ac, const char *av[], const char *env[]) {
	(void) av; (void) ac; (void) env;

	shell(SHELL_INIT);
	g_signal = 0;
	job_list = job_list_init();

	Vars *shell_vars = vars_init(env);
	int history_fd = -1;
	ShellInfos *self = shell(SHELL_GET);

	if (self->interactive) 
		history_fd = get_history();

	char *input = NULL;
	//display the prompt, init signals if shell in interactive and reads input
	while (true) {
		input = init_prompt_and_signals(input, self->interactive);
		if (self->interactive)
			do_job_notification();
		if (input) {
			if (self->interactive) {
				if (history_expansion(&input, history_fd) == false){
					continue;
				}
				add_input_to_history(input, &history_fd);
			}
			Lexer_p lexer = lexer_init(input);
			TokenList *tokens = lexer_lex_all(lexer);
			if (lexer_syntax_error(tokens))
				continue; 
			heredoc_detector(tokens);
			Node *AST = ast_build(tokens);
			ast_execute(AST, shell_vars, true);
			//update env '_' variable
			char *last_executed = ft_strjoin("_=", input);
			string_list_add_or_update(shell_vars->env, last_executed);
			string_list_add_or_update(shell_vars->set, last_executed);
			FREE_POINTERS(last_executed);
			//reset memory collection for the next input
			gc(GC_RESET, GC_SUBSHELL);
		} else {
			if (job_list->size) {
				printf("There are running jobs\n");
				job_killall();
				continue;
			}
			break;
		}
	}
	if (isatty(STDIN_FILENO))
		destroy_history();
	gc(GC_CLEANUP, GC_ALL);
	close_all_fds();
	return (EXIT_SUCCESS);
}
