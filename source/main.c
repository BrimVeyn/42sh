/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 14:55:55 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/14 15:21:57 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/utils.h"
#include "../include/signals.h"
#include "../include/debug.h"
#include "../include/lexer.h"
#include "../include/ast.h"
#include "../include/exec.h"
#include "../include/regex.h"
#include "../libftprintf/header/libft.h"
#include "ft_regex.h"
#include "ft_readline.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>

int g_debug = 0;

char *init_prompt_and_signals(void) {
	signal_manager(SIG_PROMPT);
	
	char *input = NULL;
	// if (isatty(STDIN_FILENO)){
		input = ft_readline("42sh > ");
	// }
	// else
		// input = get_next_line(STDIN_FILENO);
	return input;
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
		history_fd = get_history();

	while ((input = init_prompt_and_signals()) != NULL) {
		if (*input) 
		{	
			if (isatty(STDIN_FILENO)){
				if (history_expansion(&input, history_fd) == false){
					continue;
				}
			}
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
	if (isatty(STDIN_FILENO))
		destroy_history();
	gc_cleanup(GC_ALL);
	close_std_fds();
	return (0);
}
