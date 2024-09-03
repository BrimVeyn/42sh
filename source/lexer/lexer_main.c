/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 13:38:21 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/02 11:11:36 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"
#include "lexer.h"
#include "lexer_enum.h"
#include <stdio.h>

TokenList *lexer_lex_till(Lexer_p l, type_of_separator sep) {
	TokenList *self = token_list_init();
	while (true) {
		Token *tmp = lexer_get_next_token(l, false, DEFAULT);
		token_list_add(self, tmp);
		if (tmp->tag == T_SEPARATOR && (tmp->s_type == sep || tmp->s_type == S_EOF))
			break;
	}
	return self;
}

int main(int ac, char *av[]) {
	//Basic redirection test
	(void) ac;
	(void) av;
	gc_init();
	Token *none_token __attribute__((unused)) = genNoneTok();
	
	char *input = NULL;
	while ((input = readline("> ")) != NULL) {
		if (*input) 
		{
			Lexer_p l = lexer_init(input);
			TokenList *l1 = lexer_lex_all_test(l, S_EOF, DEFAULT);
			if (!l1) {
				printf("catched !\n");
				continue;
			}
			tokenToStringAll(l1);
			add_history(input);
		}
	}

	rl_clear_history();
	gc_cleanup();
	return (0);
}
