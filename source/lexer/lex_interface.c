/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lex_interface.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 13:04:21 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/27 15:52:59 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "utils.h"

#include <stdio.h>

bool get_next_token(StringStream *input, StringStream *cache, CursorPosition *pos);

void *lex_interface(const LexMode mode, void *input, void *filename, bool *error, Vars * const shell_vars) {
	static Lex *lexer = NULL;

	switch (mode) {
		case (LEX_OWN): return lexer;
		case (LEX_SET): {
			lexer = gc_unique(Lex, GC_SUBSHELL);
			da_create(input_ss, StringStream, sizeof(char), GC_SUBSHELL);
			da_create(raw_input_ss, StringStream, sizeof(char), GC_SUBSHELL);
			da_create(peak, StringStream, sizeof(char), GC_SUBSHELL);
			da_create(produced_tokens, TokenTypeVect, sizeof(TokenType), GC_SUBSHELL);
			ss_push_string(raw_input_ss, input);
			ss_push_string(input_ss, input);
			lexer->filename = (filename == NULL) ? "terminal" : filename;
			lexer->input = input_ss;
			lexer->peak = peak;
			lexer->raw_input_ss = raw_input_ss;
			lexer->shell_vars = shell_vars;
			lexer->pos.line = 1;
			lexer->produced_tokens = produced_tokens;
			break;
		}
		case (LEX_GET): {
			if (lexer->peak->size) {
				return ss_get_owned_slice(lexer->peak);
			} else {
				da_create(tmp, StringStream, sizeof(char), GC_SUBSHELL);
				if (!get_next_token(lexer->input, tmp, &lexer->pos))
					*error = true;
				return ss_get_owned_slice(tmp);
			}
			break;
		}
		case (LEX_PEAK): {
			if (lexer->peak->size) {
				return lexer->peak->data;
			} else {
				if (!get_next_token(lexer->input, lexer->peak, &lexer->pos))
					*error = true;
				return lexer->peak->data;
			}
			break;
		} 
		case (LEX_PEAK_CHAR): return lexer->input->data[0] != '\0' ? &lexer->input->data[0] : NULL;
		case (LEX_DEBUG): ft_dprintf(2, "input: %s\n", lexer->input->data); ft_dprintf(2, "peak: %s\n", lexer->peak->data); break;
	}
	return NULL;
}
