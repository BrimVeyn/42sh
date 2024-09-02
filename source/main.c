/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 16:22:08 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/02 16:58:53 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/42sh.h"
#include <unistd.h>

int debug = 0;
int g_signal;

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

int ft_strlenlen(char **strstr){
	int i = 0;
	for (; strstr[i]; i++){}
	return i;
}

char **ft_strdupdup(char **env){
	char **strstr = ft_calloc(ft_strlenlen(env), sizeof(char *));
	if (!strstr){
		return NULL;
	}
	for (int i = 0; env[i]; i++){
		strstr[i] = ft_strdup(env[i]);
	}
	return strstr;
}

int main(int ac, char *av[], char *env[]) {
	//Basic redirection test
	//
	if (ac != 1 && !ft_strcmp("-d", av[1])){
		debug = 1;
	}

	char **dup_env = ft_strdupdup(env);
	gc_init();
	g_signal = 0;
	Token *none_token __attribute__((unused)) = genNoneTok();
	
	char *input = NULL;

	while ((input = init_prompt_and_signals()) != NULL) {
		if (*input) 
		{
			add_history(input);
			Parser *p = parser_init(input);
			if (!syntax_error_detector(p)) {
				continue;
			}
			heredoc_detector(p);
			if (debug){
				tokenToStringAll(p->data);
				parser_print_state(p);
			}
			signal_manager(SIG_EXEC);
			parser_parse_all(p, dup_env);
		}
	}

	rl_clear_history();
	gc_cleanup();
	return (0);
}
