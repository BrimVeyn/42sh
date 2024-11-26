/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lex.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/26 14:30:02 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/26 17:33:30 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"
#include "lexer.h"
#include "utils.h"
#include "libft.h"
#include "ft_readline.h"

#include <stdio.h>
#include <time.h>

#define BIT_IS_ON(mask, bit) (((mask) & (1U << (bit))) != 0)

void printBinary(int n) {
    unsigned int mask = 1 << (sizeof(int) * 8 - 1); // Mask for the most significant bit
    for (; mask > 0; mask >>= 1) {
        if (n & mask) {
            putchar('1');
        } else {
            putchar('0');
        }
    }
    putchar('\n'); // Add a newline at the end
}

WordContext get_context(const StringStream *input, WordContextBounds *map, const WordContext context) {
    const unsigned char byteptr = map[context].bitmap;

    static const struct {
        WordContext type;
        int check_length;
    } contexts[] = {
		{WORD_WORD, 0},
        {WORD_ARITHMETIC, 3},
        {WORD_CMD_SUB, 2},
        {WORD_PARAM, 2},
        {WORD_SUBSHELL, 1},
        {WORD_ARITHMETIC_PAREN, 1},
        {WORD_SINGLE_QUOTE, 1},
        {WORD_DOUBLE_QUOTE, 1}
    };

    for (size_t i = 0; i < sizeof(contexts) / sizeof(contexts[0]); i++) {
        const WordContext current_context = contexts[i].type;
        const int length = contexts[i].check_length;

        if (BIT_IS_ON(byteptr, current_context) && contexts[i].check_length &&
            !ft_strncmp(input->data, map[current_context].start, length)) {
			// dprintf(2, "new context: %s\n", map[current_context].start);
            return current_context;
        }
    }

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

void da_transfer(StringStream *in, StringStream *out, int number) {
	for (int i = 0; i < number; i++) {
		da_push(in, da_pop_front(out));
	}
}

bool get_next_token(StringStream *input, StringStream *cache, size_t *line, size_t *column) {

	if (*input->data == '#') {
		while (input->size && *input->data != '\n') {
			da_pop_front(input);
		}
	}

	static WordContextBounds map[] = {
		[WORD_WORD] = {.start = "NONE", .end = " \t\n;&|<>()", .bitmap = WORD_MAP},
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

	while (*input->data && (*input->data == '\t' || *input->data == ' ')) {
		da_pop_front(input);
		(*column)++;
	}

	while (input->size) {

		char f1 = da_peak_front(input);
		char f2 = input->data[1];
		if (f1 == '\\' && (!f2 || f2 == '\n')) {
			da_pop_front(input); da_pop_front(input);
			if (shell(SHELL_GET)->interactive) {
				Lex *lexer = lex_interface(LEX_OWN, NULL, NULL, NULL, NULL);
				char *PS2 = string_list_get_value(lexer->shell_vars->set, "PS2");
				char *input_continuation = ft_readline(PS2, lexer->shell_vars);
				ss_push_string(input, input_continuation);
			}
		}

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
			(*line)++; (*column) = 0;
		} else {
			(*column)++;
		}
		da_push(cache, c);
	}

	if (!*cache->data) {
		if (!ft_strncmp(input->data, "<<-", 3)) { da_transfer(cache, input, 3); (*column) += 3; return true; }
		if (!ft_strncmp(input->data, "<&", 2)) { da_transfer(cache, input, 2); (*column) += 2; return true; }
		if (!ft_strncmp(input->data, ">&", 2)) { da_transfer(cache, input, 2); (*column) += 2; return true; }
		if (!ft_strncmp(input->data, "<<", 2)) { da_transfer(cache, input, 2); (*column) += 2; return true; }
		if (!ft_strncmp(input->data, "<>", 2)) { da_transfer(cache, input, 2); (*column) += 2; return true; }
		if (!ft_strncmp(input->data, ">>", 2)) { da_transfer(cache, input, 2); (*column) += 2; return true; }
		if (!ft_strncmp(input->data, ">|", 2)) { da_transfer(cache, input, 2); (*column) += 2; return true; }
		if (!ft_strncmp(input->data, ";;", 2)) { da_transfer(cache, input, 2); (*column) += 2; return true; }
		if (!ft_strncmp(input->data, "&&", 2)) { da_transfer(cache, input, 2); (*column) += 2; return true; }
		if (!ft_strncmp(input->data, "||", 2)) { da_transfer(cache, input, 2); (*column) += 2; return true; }
		switch (input->data[0]) {
			case ';' : da_push(cache, da_pop_front(input)); (*column)++; return true;
			case '&' : da_push(cache, da_pop_front(input)); (*column)++; return true;
			case '<' : da_push(cache, da_pop_front(input)); (*column)++; return true;
			case '>' : da_push(cache, da_pop_front(input)); (*column)++; return true;
			case '|' : da_push(cache, da_pop_front(input)); (*column)++; return true;
			case '(' : da_push(cache, da_pop_front(input)); (*column)++; return true;
			case ')' : da_push(cache, da_pop_front(input)); (*column)++; return true;
			case '\n': da_push(cache, da_pop_front(input)); (*line)++; (*column) = 0; return true;
		}
	}

	if (context_stack->size != 0) {
		WordContext context = da_peak_back(context_stack);	
		if (context != WORD_WORD) {
			pretty_error("eof");
			return false;
		}
	}
	return true;
}
