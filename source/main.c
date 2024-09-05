/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:00:15 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/05 09:48:42 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/42sh.h"
#include <unistd.h>

int g_debug = 0;

char *gnl() {
	char buffer[2];
	char *line = NULL;
	size_t len = 0;

	buffer[1] = '\0';

	while (read(STDIN_FILENO, buffer, 1) > 0 && buffer[0] != '\n') {
		char *tmp = realloc(line, len + 2);
		if (!tmp) {
			free(line);
			return NULL;
		}
		line = tmp;
		line[len] = buffer[0];
		len++;
	}
	if (line) {
		line[len] = '\0';
	}
	return line;
}

char *init_prompt_and_signals(void) {
	signal_manager(SIG_PROMPT);
	rl_event_hook = rl_event_dummy;
	
	char *input = NULL;
	if (isatty(STDIN_FILENO))
		input = readline("42sh > ");
	else
		input = gnl();
	return input;
}

int main(int ac, char *av[], char *env[]) {
	//Basic redirection test
	(void) ac;
	(void) av;
	(void) env;

	if (ac != 1 && !ft_strcmp("-d", av[1])){
		g_debug = 1;
	}

	char **dup_env = ft_strdupdup(env);
	(void) dup_env;
	gc_init();
	g_signal = 0;
	
	char *input = NULL;

	while ((input = init_prompt_and_signals()) != NULL) {
		if (*input) 
		{
			add_history(input);
			Lexer_p lexer = lexer_init(input);
			TokenList *tokens = lexer_lex_all(lexer);
			if (lexer_syntax_error(tokens)) continue; 
			heredoc_detector(tokens);
			signal_manager(SIG_EXEC);
			Node *AST = ast_build(tokens);
			ast_execute(AST, dup_env);
			if (g_debug){
				printTree(AST);
			}
		}
	}
	free_charchar(dup_env);
	rl_clear_history();
	gc_cleanup();
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	return (0);
}
