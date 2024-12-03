/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lex.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/26 14:30:02 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/03 18:20:46 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"
#include "lexer.h"
#include "signals.h"
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

        if (BIT_IS_ON(byteptr, current_context) &&
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

bool is_continuable(const TokenType type) {
	return (type == AND_IF || type == OR_IF || type == PIPE);
}

void line_continuation(const Lex * const lexer) {
	const ShellInfos * const shell_infos = shell(SHELL_GET);
	if (!shell_infos->interactive || shell_infos->script) return ;

	const CursorPosition * pos = &lexer->pos;
	StringStream * const input = lexer->input;

	size_t i = pos->absolute;
	while (is_whitespace(lexer->raw_input_ss->data[i])) { i++; }

	if (lexer->raw_input_ss->data[i] != '\n' && 
		lexer->raw_input_ss->data[i] != '\0') { return ; }

	const char * const PS2 = string_list_get_value(lexer->shell_vars->set, "PS2");
	signal_manager(SIG_HERE_DOC);
	char * const input_continuation = ft_readline(PS2, lexer->shell_vars);
	signal_manager(SIG_PROMPT);
	ss_push_string(input, input_continuation);
	ss_push_string(lexer->raw_input_ss, input_continuation);
	FREE_POINTERS(input_continuation);
	const char * const new_history_entry = ss_to_string(lexer->raw_input_ss);
	pop_history();
	add_history(new_history_entry, lexer->shell_vars);
}

void line_continuation_backslash(Lex *lexer, StringStream * const input, CursorPosition * const pos) {
	const char f1 = input->data[0];
	const char f2 = input->data[1];
	const ShellInfos * const shell_infos = shell(SHELL_GET);
	if (f1 == '\\' && (!f2 || f2 == '\n')) {
		da_pop_front(input); da_pop_front(input);

		if (shell_infos->interactive && !shell_infos->script) {
			const char * const PS2 = string_list_get_value(lexer->shell_vars->set, "PS2");
			signal_manager(SIG_HERE_DOC);
			char * const input_continuation = ft_readline(PS2, lexer->shell_vars);
			signal_manager(SIG_PROMPT);
			ss_push_string(input, input_continuation);
			ss_insert_string(lexer->raw_input_ss, input_continuation, pos->absolute);
			FREE_POINTERS(input_continuation);
			if (f2 == '\n') da_pop(lexer->raw_input_ss);
			da_pop(lexer->raw_input_ss);
			const char * const new_history_entry = ss_to_string(lexer->raw_input_ss);
			pop_history();
			add_history(new_history_entry, lexer->shell_vars);
		} else {
			da_erase_index(lexer->raw_input_ss, lexer->pos.absolute);
			da_erase_index(lexer->raw_input_ss, lexer->pos.absolute);
		}
	}
}

void pass_whitespace(StringStream * const input, CursorPosition * const pos) {
	while (*input->data && (*input->data == '\t' || *input->data == ' ')) {
		da_pop_front(input); (pos->column)++; (pos->absolute)++;
	}
}

bool get_next_token(Lex * const lexer, StringStream * const cache) {

	StringStream * const input = lexer->input;
	CursorPosition * const  pos = &lexer->pos;

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

	pass_whitespace(input, pos);
	line_continuation_backslash(lexer, input, pos);
	pass_whitespace(input, pos);

	if (*input->data == '#') {
		while (input->size && *input->data != '\n' && *input->data != '\0') {
			da_pop_front(input);
		}
	}

	while (input->size) {

		line_continuation_backslash(lexer, input, pos);

		const WordContext maybe_new_context = get_context(input, map, da_peak_back(context_stack));

		if (maybe_new_context != NONE) {
			da_push(context_stack, maybe_new_context);
			for (size_t i = 0; i < ft_strlen(map[maybe_new_context].start); i++) {
				da_push(cache, da_pop_front(input));
				(pos->column)++; (pos->absolute)++;
			}
			continue;
		}

		const WordContext mayber_end_of_context = get_end_of_context(input, map, da_peak_back(context_stack));

		if (mayber_end_of_context != NONE) {
			da_pop(context_stack);
			if (!context_stack->size) {
				break;
			}
			for (size_t i = 0; i < ft_strlen(map[mayber_end_of_context].end); i++) {
				da_push(cache, da_pop_front(input));
				(pos->column)++; (pos->absolute)++;
			}
			continue;
		}

		const char c = da_pop_front(input);
		if (c == '\n') { (pos->line)++; (pos->column) = 0; (pos->absolute)++; } 
		else { (pos->column)++; (pos->absolute)++; }

		da_push(cache, c);
	}

	if (!*cache->data) {
		if (!ft_strncmp(input->data, "<<-", 3)) { da_transfer(cache, input, 3); (pos->column) += 3; (pos->absolute) += 3; return true; }
		if (!ft_strncmp(input->data, "<&", 2)) { da_transfer(cache, input, 2); (pos->column) += 2; (pos->absolute) += 2; return true; }
		if (!ft_strncmp(input->data, ">&", 2)) { da_transfer(cache, input, 2); (pos->column) += 2; (pos->absolute) += 2; return true; }
		if (!ft_strncmp(input->data, "<<", 2)) { da_transfer(cache, input, 2); (pos->column) += 2; (pos->absolute) += 2; return true; }
		if (!ft_strncmp(input->data, "<>", 2)) { da_transfer(cache, input, 2); (pos->column) += 2; (pos->absolute) += 2; return true; }
		if (!ft_strncmp(input->data, ">>", 2)) { da_transfer(cache, input, 2); (pos->column) += 2; (pos->absolute) += 2; return true; }
		if (!ft_strncmp(input->data, ">|", 2)) { da_transfer(cache, input, 2); (pos->column) += 2; (pos->absolute) += 2; return true; }
		if (!ft_strncmp(input->data, ";;", 2)) { da_transfer(cache, input, 2); (pos->column) += 2; (pos->absolute) += 2; return true; }
		if (!ft_strncmp(input->data, "&&", 2)) { da_transfer(cache, input, 2); (pos->column) += 2; (pos->absolute) += 2; return true; }
		if (!ft_strncmp(input->data, "||", 2)) { da_transfer(cache, input, 2); (pos->column) += 2; (pos->absolute) += 2; return true; }
		switch (input->data[0]) {
			case ';' : da_push(cache, da_pop_front(input)); (pos->column)++; (pos->absolute)++; return true;
			case '&' : da_push(cache, da_pop_front(input)); (pos->column)++; (pos->absolute)++; return true;
			case '<' : da_push(cache, da_pop_front(input)); (pos->column)++; (pos->absolute)++; return true;
			case '>' : da_push(cache, da_pop_front(input)); (pos->column)++; (pos->absolute)++; return true;
			case '|' : da_push(cache, da_pop_front(input)); (pos->column)++; (pos->absolute)++; return true;
			case '(' : da_push(cache, da_pop_front(input)); (pos->column)++; (pos->absolute)++; return true;
			case ')' : da_push(cache, da_pop_front(input)); (pos->column)++; (pos->absolute)++; return true;
			case '\n': da_push(cache, da_pop_front(input)); (pos->line)++; (pos->column) = 0; (pos->absolute)++; return true;
		}
	}

	if (context_stack->size != 0) {
		const WordContext context = da_peak_back(context_stack);	
		if (context != WORD_WORD) { pretty_error(lexer, ""); return false; }
	}
	return true;
}
