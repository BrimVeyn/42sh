/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:08:53 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/16 13:30:18 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "lexer_enum.h"
#include <stdint.h>

char *get_tagName_redirection(type_of_redirection type) {
	switch(type) {
		case R_HERE_DOC:
			return "HERE_DOC";
		case R_AMPERSAND:
			return "AMPERSAND";
		case R_INPUT_FILE:
			return "INPUT_FILE";
		case R_OUTPUT_FILE:
			return "OUTPUT_FILE";
		case R_APPEND_FILE:
			return "OUTPUT_FILE_APPEND";
		default:
			return "UNKNOWN_TYPE";
	}
}

int main(void) {
	char *input = strdup(" <file <file echo \"salut\"");

	Lexer_p lexer = lexer_init(input);
	lexer_debug(lexer);

	// Token *tokens;
	// while (true) {
	// }
	Token tok = lexer_get_next_token(lexer);

	printf("filename = |%s|\n", tok.filename);
	printf("type = %s\n", tagName(tok.r_type));
	free(tok.filename);

	lexer_deinit(lexer);
}
