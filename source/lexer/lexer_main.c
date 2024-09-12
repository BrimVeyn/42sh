/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 13:38:21 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/12 10:02:47 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"
int g_debug = 0;

int main(int ac, char *av[]) {
	//Basic redirection test
	(void) ac;
	(void) av;
	gc_init();
	Token *none_token __attribute__((unused)) = token_none_init();
	
	char *input = NULL;
	while ((input = readline("> ")) != NULL) {
		if (*input) 
		{
			Lexer_p l = lexer_init(input);
			TokenList *l1 = lexer_lex_all(l);
			if (!l1) continue;
			tokenToStringAll(l1);
			add_history(input);
		}
	}

	rl_clear_history();
	gc_cleanup();
	return (0);
}
