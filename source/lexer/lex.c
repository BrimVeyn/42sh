/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lex.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/26 14:30:02 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/30 12:21:43 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "utils.h"
#include "libft.h"
#include "debug.h"
#include <stdio.h>

#define BITMAP8_SIZE 8
#define IS_ON(ptr, context) ((*ptr) & (1 << (int) context)) > 0

WordContext get_context(const StringStream *input, WordContextBounds *map, const WordContext context) {
	const unsigned char *byteptr = (unsigned char *)&map[context].bitmap;

	if (IS_ON(byteptr, WORD_ARITHMETIC) && !ft_strncmp(input->data, map[WORD_ARITHMETIC].start, 3)) 
		return WORD_ARITHMETIC;
	if (IS_ON(byteptr, WORD_CMD_SUB) && !ft_strncmp(input->data, map[WORD_CMD_SUB].start, 2)) 
		return WORD_CMD_SUB;
	if (IS_ON(byteptr, WORD_PARAM) && !ft_strncmp(input->data, map[WORD_PARAM].start, 2)) 
		return WORD_PARAM;
	if (IS_ON(byteptr, WORD_SUBSHELL) && !ft_strncmp(input->data, map[WORD_SUBSHELL].start, 1)) 
		return WORD_SUBSHELL;
	if (IS_ON(byteptr, WORD_ARITHMETIC_PAREN) && !ft_strncmp(input->data, map[WORD_ARITHMETIC_PAREN].start, 1)) 
		return WORD_ARITHMETIC_PAREN;
	if (IS_ON(byteptr, WORD_SINGLE_QUOTE) && !ft_strncmp(input->data, map[WORD_SINGLE_QUOTE].start, 1)) 
		return WORD_SINGLE_QUOTE;
	if (IS_ON(byteptr, WORD_DOUBLE_QUOTE) && !ft_strncmp(input->data, map[WORD_DOUBLE_QUOTE].start, 1)) 
		return WORD_DOUBLE_QUOTE;
	return NONE;
}

WordContext get_end_of_context(const StringStream *input, WordContextBounds *map, const WordContext context) {
	if (context == WORD_WORD && ft_strchr(map[context].end, input->data[0])) {
		return context;
	} else if (context == WORD_WORD) {
		return NONE;
	}

	if (!ft_strncmp(input->data, map[context].end, ft_strlen(map[context].end))) {
		return context;
	} else {
		return NONE;
	}
}

void get_next_token(StringStream *input, StringStream *cache, size_t *line, size_t *column) {

	static WordContextBounds map[] = {
		[WORD_WORD] = {.start = "NONE", .end = " \t\n;&", .bitmap = WORD_MAP},
		[WORD_CMD_SUB] = {"$(", ")", CMD_SUB_MAP},
		[WORD_PARAM] = { "${", "}" , PARAM_MAP},
		[WORD_SUBSHELL] = { "(", ")", SUBSHELL_MAP},
		[WORD_ARITHMETIC] = {"$((", "))" , ARITHMETIC_MAP},
		[WORD_ARITHMETIC_PAREN] = { "(", ")" , ARITHMETIC_PAREN_MAP},
		[WORD_SINGLE_QUOTE] = {"'", "'", SINGLE_QUOTE_MAP},
		[WORD_DOUBLE_QUOTE] = {"\"", "\"", DOUBLE_QUOTE_MAP},
	};

	da_create(context_stack, WordContextList, sizeof(WordContext), GC_SUBSHELL);
	da_push(context_stack, WORD_WORD);

	while (input->size) {
		WordContext maybe_new_context = get_context(input, map, da_peak_back(context_stack));

		if (maybe_new_context != NONE) {
			da_push(context_stack, maybe_new_context);
			for (size_t i = 0; i < ft_strlen(map[maybe_new_context].start); i++) {
				da_push(cache, da_pop_front(input));
				(*column)++;
			}
			continue;
		}

		WordContext mayber_end_of_context = get_end_of_context(input, map, da_peak_back(context_stack));

		if (mayber_end_of_context != NONE) {
			da_pop(context_stack);
			if (!context_stack->size) {
				break;
			}
			for (size_t i = 0; i < ft_strlen(map[mayber_end_of_context].end); i++) {
				da_push(cache, da_pop_front(input));
				(*column)++;
			}
			continue;
		}

		char c = da_pop_front(input);
		if (c == '\n') {
			(*line)++;
			(*column) = 0;
		} else {
			(*column)++;
		}
		da_push(cache, c);
	}

	if (!*cache->data) {
		if (!ft_strncmp(input->data, "&&", 2)) {
			da_push(cache, da_pop_front(input));
			da_push(cache, da_pop_front(input));
			(*column)++;
			(*column)++;
			return ;
		} else if (!ft_strncmp(input->data, "||", 2)) {
			da_push(cache, da_pop_front(input));
			da_push(cache, da_pop_front(input));
			(*column)++;
			(*column)++;
			return ;
		}
		switch (input->data[0]) {
			case ';' : da_push(cache, da_pop_front(input)); (*column)++; return;
			case '&' : da_push(cache, da_pop_front(input)); (*column)++; return;
		}
	}

	if (context_stack->size != 0 && da_peak_back(context_stack) != WORD_WORD) {
		dprintf(2, "SYNTAX ERROR UNCLOSED SHIT\n");
	}
}
