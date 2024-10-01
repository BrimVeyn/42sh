/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 14:55:55 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/30 16:24:48 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/utils.h"
#include "../include/signals.h"
#include "../include/debug.h"
#include "../include/lexer.h"
#include "../include/ast.h"
#include "../include/exec.h"
#include "../libftprintf/header/libft.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/stat.h>
#include <sys/mman.h>

int g_debug = 0;

char *init_prompt_and_signals(void) {
	signal_manager(SIG_PROMPT);
	rl_event_hook = rl_event_dummy;
	
	char *input = NULL;
	if (isatty(STDIN_FILENO))
		input = readline("42sh > ");
	else
		input = get_next_line(STDIN_FILENO);
	return input;
}

void get_history() {
	char *home = getenv("HOME");
	char history_filename[1024] = {0};
	ft_sprintf(history_filename, "%s/.42sh_history", home);
    int fd = open(history_filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("Can't open history file");
        exit(EXIT_FAILURE);
    }

	struct stat st;
	if (fstat(fd, &st) == -1){
        perror("Can't get history's file stats");
        exit(EXIT_FAILURE);
	}
    size_t file_size = st.st_size;

    char *buffer = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buffer == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

	char *start = buffer;
    char *end = buffer; 

	while(*end){
		if (*end == '\n'){
			char *tmp = (char *)malloc(end - start + 1);
			memcpy(tmp, start, end - start);
			tmp[end - start] = '\0';
			add_history(tmp);
			free(tmp);
			start = end + 1;
		}
		end++;
	}
	munmap(buffer, file_size);
	close(fd);
}

void add_input_to_history(char *input, int *history_fd){
	add_history(input);
	char *home = getenv("HOME");
	char history_filename[1024] = {0};
	ft_sprintf(history_filename, "%s/.42sh_history", home);
	if (*history_fd == -1){
		*history_fd = open(history_filename, O_APPEND | O_WRONLY | O_CREAT, 0644);
		if (*history_fd == -1) {
			perror("Can't open history file");
			exit(EXIT_FAILURE);
		}
	}
	dprintf(*history_fd, "%s\n", input);
}

void env_to_string_list(StringList *env_list, const char **env){
	for (uint16_t i = 0; env[i]; i++)
		string_list_add_or_update(env_list, gc_add(ft_strdup(env[i]), GC_SUBSHELL));
}

Vars *vars_init(const char **env) {
	Vars *self = gc_add(ft_calloc(1, sizeof(Vars)), GC_SUBSHELL);

	self->env = string_list_init();
	self->set = string_list_init();
	env_to_string_list(self->env, env);
	env_to_string_list(self->set, env);

	return self;
}

int main(const int ac, const char *av[], const char *env[]) {

	if (ac != 1 && !ft_strcmp("-d", av[1])){
		g_debug = 1;
	}

	gc_init(GC_GENERAL);
	gc_init(GC_SUBSHELL);
	Vars *shell_vars = vars_init(env);
	int history_fd = -1;

	g_signal = 0;
	char *input = NULL;

	if (isatty(STDIN_FILENO))
		get_history();

	while ((input = init_prompt_and_signals()) != NULL) {
		if (*input) 
		{
			if (isatty(STDIN_FILENO)){
				add_input_to_history(input, &history_fd);
			}
			input = replace_char_greedy(input, '\n', ';');
			Lexer_p lexer = lexer_init(input);
			TokenList *tokens = lexer_lex_all(lexer);
			if (lexer_syntax_error(tokens))
				continue; 
			heredoc_detector(tokens);
			signal_manager(SIG_EXEC);
			Node *AST = ast_build(tokens);
			ast_execute(AST, shell_vars);
			if (g_debug){
				printTree(AST);
			}
			// gc_cleanup(GC_SUBSHELL);
			// gc_init(GC_GENERAL);
		}
	}
	rl_clear_history();
	gc_cleanup(GC_ALL);
	close_std_fds();
	return (0);
}
