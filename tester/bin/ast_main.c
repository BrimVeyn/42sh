/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_main.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 11:47:59 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/02 12:55:19 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "signals.h"
#include "ast.h"
#include "debug.h"

#include <readline/history.h>
#include <readline/readline.h>


int g_debug = 0;

int main(int ac, char *av[]) {
	//Basic redirection test
	(void) ac;
	(void) av;
	gc_init(GC_GENERAL);
	gc_init(GC_SUBSHELL);
	
	char *input = NULL;
	while ((input = readline("> ")) != NULL) {
		if (*input) 
		{
			Lexer_p lexer = lexer_init(input);
			TokenList *tokens = lexer_lex_all(lexer);
			if (lexer_syntax_error(tokens)) continue; 
			heredoc_detector(tokens);
			signal_manager(SIG_EXEC);
			Node *AST = ast_build(tokens);
			printTree(AST);
			add_history(input);
		}
	}

	rl_clear_history();
	gc(GC_CLEANUP, GC_GENERAL);
	return (0);
}
