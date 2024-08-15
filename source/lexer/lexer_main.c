/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:08:53 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/15 17:04:32 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include <stdint.h>

int main(void) {
	char *input = strdup(" <file <file echo \"salut\"");

	Lexer_p lexer = lexer_init(input);
	lexer_debug(lexer);

	Token tok = lexer_get_next_token(lexer);

	printf("filename = |%s|\n", tok.redir.filename);
	printf("type = %d\n", tok.redir.type);
	free(tok.redir.filename);

	lexer_deinit(lexer);
}
