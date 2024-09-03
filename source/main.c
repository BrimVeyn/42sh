/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 16:22:08 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/03 13:57:29 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/42sh.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

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
			Lexer_p lexer = lexer_init(input);
			TokenList *tokens = lexer_lex_all(lexer);
			heredoc_detector(tokens);
			signal_manager(SIG_EXEC);
			Node *AST = ast_build(tokens);
			printTree(AST);
		}
	}

	rl_clear_history();
	gc_cleanup();
	return (0);
}
