/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/14 16:04:41 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/30 16:12:35 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/42sh.h"

int g_signal;

char *init_prompt_and_signals(void) {
	signal_manager(SIG_PROMPT);
	rl_event_hook = rl_event_dummy;
	char *input = readline("42sh > ");
	return input;
}

int main(int ac, char *av[], char *env[]) {
	//Basic redirection test
	(void) ac;
	(void) av;
	(void) env;
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
			// here_doc(); LARBIN
			tokenToStringAll(p->data);
			parser_print_state(p);
			signal_manager(SIG_EXEC);
			parser_parse_all(p);
		}
	}

	rl_clear_history();
	gc_cleanup();
	return (0);
}
