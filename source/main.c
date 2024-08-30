/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/14 16:04:41 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/30 13:50:28 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/42sh.h"
#include "lexer/lexer_enum.h"
#include <stdio.h>

int main(int ac, char *av[], char *env[]) {
	//Basic redirection test
	(void) ac;
	(void) av;
	(void) env;
	gc_init();
	Token *none_token __attribute__((unused)) = genNoneTok();
	
	char *input = NULL;
	while ((input = readline("42sh > ")) != NULL) {
		if (*input) 
		{
			add_history(input);
			Parser *p = parser_init(input);
			if (!syntax_error_detector(p)) {
				continue;
			}
			tokenToStringAll(p->data);
			parser_print_state(p);
			parser_parse_all(p);
		}
	}

	rl_clear_history();
	gc_cleanup();
	return (0);
}
