/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lex_interface.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 13:04:21 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/24 16:22:04 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "utils.h"

#include <stdio.h>

bool get_next_token(StringStream *input, StringStream *cache, size_t *line, size_t *column);

void *lex_interface(const LexMode mode, void *input, void *filename, bool *error) {
	static Lex *lexer = NULL;

	switch (mode) {
		case (LEX_OWN): return lexer;
		case (LEX_SET): {
			lexer = gc_unique(Lex, GC_SUBSHELL);
			lexer->raw_input = input;
			lexer->filename = (filename == NULL) ? "terminal" : filename;
			da_create(tmp, StringStream, sizeof(char), GC_SUBSHELL);
			lexer->input = tmp;
			da_create(tmp2, StringStream, sizeof(char), GC_SUBSHELL);
			lexer->peak = tmp2;
			lexer->line = 1;
			ss_push_string(lexer->input, input);
			break;
		}
		case (LEX_GET): {
			if (lexer->peak->size) {
				return ss_get_owned_slice(lexer->peak);
			} else {
				da_create(tmp, StringStream, sizeof(char), GC_SUBSHELL);
				if (!get_next_token(lexer->input, tmp, &lexer->line, &lexer->column))
					*error = true;
				return ss_get_owned_slice(tmp);
			}
			break;
		}
		case (LEX_PEAK): {
			if (lexer->peak->size) {
				return lexer->peak->data;
			} else {
				if (!get_next_token(lexer->input, lexer->peak, &lexer->line, &lexer->column))
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
