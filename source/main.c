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
typedef enum { ARGS_GET, ARGS_SET } args_mode;

void	*init_prompt_and_signals(char *input, bool shell_is_interactive, Vars *shell_vars) {
	if (shell_is_interactive)
		signal_manager(SIG_PROMPT);
	
	char *prompt = string_list_get_value(shell_vars->set, "PS1");
	if (prompt)
		input = ft_readline(prompt);
	else
		input = ft_readline("42sh> ");
	if (!input) {
		return NULL;
	}
	gc(GC_ADD, input, GC_SUBSHELL);
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

int manage_ac(args_mode mode, int new_ac){
	static int ac;
	if (mode == ARGS_GET)
		return ac;
	else if (mode == ARGS_SET){
		ac = new_ac;
		return ac;
	}
	return 0;
}

/*
	* da_create(pos_args, StringList, sizeof(char *), GC_SUBSHELL);
	* da_push(string)
	* da_pop();
	*
	*
	*
	*
*/

char *open_read_file(char *path){
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
	int ac = manage_ac(ARGS_GET, 0);

	if (mode == SHELL_INIT) {
		self = gc(GC_CALLOC, 1, sizeof(ShellInfos), GC_ENV);
		self->shell_terminal = STDIN_FILENO;
		self->interactive = isatty(self->shell_terminal);
		if (ac > 1){
			self->interactive = 2;
		}

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

//TODO:Default file propre, syntax errors for 42shrc
void exec_config_file(Vars *shell_vars) {
    char *home = string_list_get_value(shell_vars->env, "HOME");
    char config_filename[1024] = {0};
    ft_sprintf(config_filename, "%s/.42shrc", home);

    int fd = open(config_filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("Can't open config file");
        return;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("Can't get config's file stats");
        close(fd);
        return;
    }

    size_t file_size = st.st_size;

    if (file_size == 0) {
		// write(fd, "PS1=42sh", 9); //tmp
		// execute_command_sub("PS1=42sh", shell_vars);
		close(fd);
		return;
    }

    char *buffer = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buffer == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        return;
    }
	
	Lexer_p lexer = lexer_init(buffer);
	TokenList *tokens = lexer_lex_all(lexer);
	// if (lexer_syntax_error(tokens))
	// 	clean_sub();
	// heredoc_detector(tokens);
	Node *AST = ast_build(tokens);
	ast_execute(AST, shell_vars, true);

    munmap(buffer, file_size);
    close(fd);
}


//TODO:string_list args

/*
	* da_create(pos_args, StringList, sizeof(char *), GC_SUBSHELL);
	* da_push(string)
	* da_pop();
	*
	*
	*
	*
*/

int main(const int ac, char *av[], const char *env[]) {
	(void) av; (void) ac; (void) env;
	
	manage_ac(ARGS_SET, ac);
	shell(SHELL_INIT);
	g_signal = 0;
	job_list = job_list_init();

	Vars *shell_vars = vars_init(env);
	int history_fd = -1;
	ShellInfos *self = shell(SHELL_GET);

	if (self->interactive){
		history_fd = get_history();
		signal_manager(SIG_PROMPT);
		exec_config_file(shell_vars);
	}

	char *input = NULL;
	//display the prompt, init signals if shell in interactive and reads input
	while (true) {
		if (self->interactive < 2){
			input = init_prompt_and_signals(input, self->interactive, shell_vars);
		} else {
			input = open_read_file(av[1]);
			if (ac > 2){
				da_create(pos_args, StringList, sizeof(char *), GC_SUBSHELL);//definition of pos_args
				for (int i = 2; i < ac; i++){
					da_push(pos_args, av[i]);
				}
			}
		}
		if (self->interactive)
			do_job_notification();
		if (input) {
			if (self->interactive) {
				if (history_expansion(&input, history_fd) == false){
					continue;
				}
				if (input[0])
					add_input_to_history(input, &history_fd);
			}
			Lexer_p lexer = lexer_init(input);
			TokenList *tokens = lexer_lex_all(lexer);
			// tokenListToString(tokens);
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
		if (self->interactive == 2)
			break;
	}
	if (self->interactive)
		destroy_history();
	gc(GC_CLEANUP, GC_ALL);
	close_all_fds();
	return (EXIT_SUCCESS);
}
