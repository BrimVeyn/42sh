/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 13:38:21 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/02 12:55:18 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "utils.h"
#include "debug.h"

#include <readline/readline.h>
#include <readline/history.h>

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
			Lexer_p l = lexer_init(input);
			TokenList *l1 = lexer_lex_all(l);
			if (!l1) continue;
			tokenListToString(l1);
			add_history(input);
		}
	}

	rl_clear_history();
	gc(GC_CLEANUP, GC_GENERAL);
	return (0);
}
