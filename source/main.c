/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 15:04:50 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/16 13:19:32 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/42sh.h"
#include "lexer/lexer.h"
#include <stdint.h>

int g_debug = 0;


char *init_prompt_and_signals(void) {
	signal_manager(SIG_PROMPT);
	rl_event_hook = rl_event_dummy;
	
	char *input = NULL;
	if (isatty(STDIN_FILENO))
		input = readline("42sh > ");
	else
		input = gnl(STDIN_FILENO);
	return input;
}

void get_history() {
	DIR *dir_history = opendir("/tmp/42sh");
	if (dir_history == NULL)
	{
		if (mkdir("/tmp/42sh", 0755) == -1){
			perror("Can't create cache directory");
		}
	}
	closedir(dir_history);
    int fd = open("/tmp/42sh/history.log", O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("Can't open history file");
        exit(EXIT_FAILURE);
    }

    int n = 2;

    ssize_t read_bytes = 0;
    ssize_t total = 0;

	char *buffer = ft_calloc(n + 1, sizeof(char));
	if (!buffer) {
		perror("Calloc buffer history failed");
		close(fd);
		exit(EXIT_FAILURE);
	}

	while ((read_bytes = read(fd, buffer + total, n - total)) > 0) {
		total += read_bytes;
		buffer[total] = '\0';
		
		if (total >= n - 1) {
			n *= 2;
			char *new_buffer = ft_realloc(buffer, ft_strlen(buffer), n + 1, sizeof(char));

			if (!new_buffer) {
				perror("buffer realloc history failed");
				free(buffer);
				close(fd);
				exit(EXIT_FAILURE);
			}
			ft_memset(new_buffer + total, 0, n - total);
			free(buffer);
			buffer = new_buffer;
		}
	}

    if (read_bytes == -1) {
        perror("Error read_bytesing file");
        free(buffer);
        close(fd);
        exit(EXIT_FAILURE);
    }

	// printf("buffer: |%s|", buffer);
	// ft_strlen(buffer);
	char **history = ft_split(buffer, '\n');
	for ( uint32_t i = 0; history[i]; i++) {
		add_history(history[i]);
	}

    free(buffer);
	free_charchar(history);
	close(fd);
}

void add_input_to_history(char *input){
	add_history(input);
	int history_fd = open("/tmp/42sh/history.log", O_RDWR | O_APPEND, 0644);
	if (history_fd == -1) {
		perror("Can't open history file");
		exit(EXIT_FAILURE);
	}
	dprintf(history_fd, "%s\n", input);
	close(history_fd);
}

void env_to_string_list(StringList *env_list, const char **env){
	for (uint16_t i = 0; env[i]; i++){
		string_list_add_or_update(env_list, gc_add(ft_strdup(env[i]), GC_GENERAL));
	}
	// string_list_add_or_update(env_list, NULL);
}

int main(const int ac, const char *av[], const char *env[]) {
	//Basic redirection test
	(void) ac;
	(void) av;
	(void) env;

	if (ac != 1 && !ft_strcmp("-d", av[1])){
		g_debug = 1;
	}

	gc_init(GC_GENERAL);
	gc_init(GC_SUBSHELL);
	StringList *env_list = string_list_init();
	env_to_string_list(env_list, env);
	// for(int i = 0; env_list->value[i]; i++){printf("%s", env_list->value[i]);}

	g_signal = 0;

	char *input = NULL;

	if (isatty(STDIN_FILENO))
		get_history();

	while ((input = init_prompt_and_signals()) != NULL) {
		if (*input) 
		{
			if (isatty(STDIN_FILENO)){
				add_input_to_history(input);
			}
			input = replace_char_greedy(input, '\n', ';');
			// printf("input: %s\n", input);
			Lexer_p lexer = lexer_init(input);
			TokenList *tokens = lexer_lex_all(lexer);
			if (lexer_syntax_error(tokens)) continue; 
			heredoc_detector(tokens);
			signal_manager(SIG_EXEC);
			Node *AST = ast_build(tokens);
			ast_execute(AST, env_list);
			if (g_debug){
				printTree(AST);
			}
			// gc_cleanup(GC_SUBSHELL);
			// gc_init(GC_GENERAL);
		}
	}
	rl_clear_history();
	gc_cleanup(GC_ALL);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	return (0);
}
