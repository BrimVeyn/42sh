/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_interface.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 13:04:21 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/04 11:51:30 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "final_parser.h"

#include <stdio.h>

void get_next_token(StringStream *input, StringStream *cache, size_t *line, size_t *column);

typedef struct {
	StringStream *input;
	StringStream *peak;
	size_t line; 
	size_t column;
} Lex;

char *lex_interface(LexMode mode, void *input) {
	static Lex *lexer = NULL;

	switch (mode) {
		case (LEX_SET): {
			lexer = gc_unique(Lex, GC_SUBSHELL);
			da_create(tmp, StringStream, sizeof(char), GC_SUBSHELL);
			lexer->input = tmp;
			da_create(tmp2, StringStream, sizeof(char), GC_SUBSHELL);
			lexer->peak = tmp2;
			ss_push_string(lexer->input, input);
			break;
		}
		case (LEX_GET): {
			if (lexer->peak->size) {
				return ss_get_owned_slice(lexer->peak);
			} else {
				da_create(tmp, StringStream, sizeof(char), GC_SUBSHELL);
				get_next_token(lexer->input, tmp, &lexer->line, &lexer->column);
				while (da_peak_front(lexer->input) == ' ') {
					da_pop_front(lexer->input);
				}
				return tmp->data;
			}
			break;
		}
		case (LEX_PEAK): {
			if (lexer->peak->size) {
				return lexer->peak->data;
			} else {
				get_next_token(lexer->input, lexer->peak, &lexer->line, &lexer->column);
				while (da_peak_front(lexer->input) == ' ') {
					da_pop_front(lexer->input);
				}
				return lexer->peak->data;
			}
			break;
		} 
		case (LEX_PEAK_CHAR):{
			return lexer->input->data[0] != '\0' ? &lexer->input->data[0] : NULL;
		} 
		case (LEX_DEBUG): {
			dprintf(2, "input: %s\n", lexer->input->data);
			dprintf(2, "peak: %s\n", lexer->peak->data);
			break;
		}
	}
	return NULL;
}
