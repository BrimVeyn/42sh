/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lex_interface.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 13:04:21 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/29 23:35:51 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "utils.h"
#include "dynamic_arrays.h"

#include <stdio.h>

bool get_next_token(Lex * const lexer, StringStream *const input, StringStream * const cache);

Lex *lex_init(const char * const input, char * const filename, Vars * const shell_vars) {
	Lex *lexer = gc_unique(Lex, GC_SUBSHELL);

	da_create(input_ss, StringStream, sizeof(char), GC_SUBSHELL);
	da_create(raw_input_ss, StringStream, sizeof(char), GC_SUBSHELL);
	da_create(peak_ss, StringStream, sizeof(char), GC_SUBSHELL);
	da_create(produced_tokens, TokenTypeVect, sizeof(TokenType), GC_SUBSHELL);
	da_create(alias_names, StringList, sizeof(char *), GC_SUBSHELL);
	da_create(alias_offsets, IntList, sizeof(int), GC_SUBSHELL);

	ss_push_string(raw_input_ss, input);
	ss_push_string(input_ss, input);

	lexer->input = input_ss;
	lexer->peak = peak_ss;
	lexer->raw_input_ss = raw_input_ss;
	lexer->active_aliases.names = alias_names;
	lexer->active_aliases.offsets = alias_offsets;

	lexer->filename = (filename == NULL) ? "42sh" : filename;
	lexer->produced_tokens = produced_tokens;
	lexer->shell_vars = shell_vars;
	lexer->pos.line = 1;

	return lexer;
}

void remove_active_aliases(Lex *const lexer) {
	size_t last_alias_index = lexer->active_aliases.names->size - 1;
	size_t last_offset = lexer->active_aliases.offsets->data[last_alias_index];

	if (lexer->pos.absolute >= last_offset) {
		da_pop(lexer->active_aliases.names);
		da_pop(lexer->active_aliases.offsets);
	}
}

char *lexer_get(Lex * const lexer, bool * const error) {
	if (lexer->peak->size) {
		return ss_get_owned_slice(lexer->peak);
	} else {
		da_create(tmp, StringStream, sizeof(char), GC_SUBSHELL);
		if (!get_next_token(lexer, lexer->input, tmp))
			*error = true;
		return ss_get_owned_slice(tmp);
	}
}

char *lexer_peak(Lex * const lexer, bool * const error) {
	if (lexer->peak->size) {
		return lexer->peak->data;
	} else {
		if (!get_next_token(lexer, lexer->input, lexer->peak))
			*error = true;
		return lexer->peak->data;
	}
}

char *lexer_peak_char(Lex * const lexer) {
	return lexer->input->data[0] != '\0' ? &lexer->input->data[0] : NULL;
}
