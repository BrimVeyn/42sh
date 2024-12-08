/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lex_interface.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 13:04:21 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/08 12:47:26 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "utils.h"

#include <stdio.h>

bool get_next_token(Lex * const lexer, StringStream * const cache);

Lex *lex_init(const char * const input, char * const filename, Vars * const shell_vars) {
	Lex *lexer = gc_unique(Lex, GC_SUBSHELL);
	da_create(input_ss, StringStream, sizeof(char), GC_SUBSHELL);
	da_create(raw_input_ss, StringStream, sizeof(char), GC_SUBSHELL);
	da_create(peak, StringStream, sizeof(char), GC_SUBSHELL);
	da_create(produced_tokens, TokenTypeVect, sizeof(TokenType), GC_SUBSHELL);
	ss_push_string(raw_input_ss, input);
	ss_push_string(input_ss, input);
	lexer->filename = (filename == NULL) ? "42sh" : filename;
	lexer->input = input_ss;
	lexer->peak = peak;
	lexer->raw_input_ss = raw_input_ss;
	lexer->shell_vars = shell_vars;
	lexer->pos.line = 1;
	lexer->produced_tokens = produced_tokens;
	return lexer;
}

char *lexer_get(Lex * const lexer, bool * const error) {
	if (lexer->peak->size) {
		return ss_get_owned_slice(lexer->peak);
	} else {
		da_create(tmp, StringStream, sizeof(char), GC_SUBSHELL);
		if (!get_next_token(lexer, tmp))
			*error = true;
		return ss_get_owned_slice(tmp);
	}
}

char *lexer_peak(Lex * const lexer, bool * const error) {
	if (lexer->peak->size) {
		return lexer->peak->data;
	} else {
		if (!get_next_token(lexer, lexer->peak))
			*error = true;
		return lexer->peak->data;
	}
}

char *lexer_peak_char(Lex * const lexer) {
	return lexer->input->data[0] != '\0' ? &lexer->input->data[0] : NULL;
}
