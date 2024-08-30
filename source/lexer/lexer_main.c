/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 13:38:21 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/30 11:17:32 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "../parser/parser.h"

int main(int ac, char *av[]) {
	//Basic redirection test
	gc_init();
	Token *none_token __attribute__((unused)) = genNoneTok();
	
	if (ac == 2 && !ft_strcmp("-i", av[1])) {
		char *input = NULL;
		while ((input = readline("> ")) != NULL) {
			if (*input) 
			{
				Lexer_p l = lexer_init(input, DEFAULT);
				TokenList *l1 = lexer_lex_all(l);
				tokenToStringAll(l1);
				add_history(input);
				Parser *p = parser_init(input);
				parser_print_state(p);
				parser_parse_all(p);
			}
		}
	}

	rl_clear_history();
	gc_cleanup();
	return (0);
}
