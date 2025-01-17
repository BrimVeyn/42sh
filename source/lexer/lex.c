/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lex.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/26 14:30:02 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/29 23:35:40 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"
#include "lexer.h"
#include "signals.h"
#include "utils.h"
#include "libft.h"
#include "ft_readline.h"
#include "dynamic_arrays.h"

#include <stdint.h>
#include <stdio.h>
#include <time.h>

WordContext get_context(const StringStream *input, WordContextBounds *map, const WordContext context) {
    const uint16_t byteptr = map[context].bitmap;

    for (size_t i = 1; i < 10; i++) {
		char *ctx_sstr = map[i].start;
		uint16_t mask = (1 << i);

		if (!ft_strncmp("<(", input->data, 2)) {
			/*dprintf(2, "byteptr: %d\n", map[context].bitmap);*/
			/*dprintf(2, "bitmask: %d\n", map[i].bitmap);*/
			/*dprintf(2, "VALUE: %s\n", boolStr((byteptr & mask) > 0));*/
			/*dprintf(2, "MASK: %d\n", mask);*/
		}
        if ((byteptr & mask) && !ft_strncmp(input->data, ctx_sstr, ft_strlen(ctx_sstr))) 
		{
            return i;
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

bool get_next_token(Lex * const lexer, StringStream *const input, StringStream * const cache) {

	CursorPosition * const  pos = &lexer->pos;

	static WordContextBounds map[] = {
		[WORD_WORD] = { "NONE", " \t\n;&|<>()", WORD_MAP},
		[WORD_CMD_SUB] = {"$(", ")", CMD_SUB_MAP},
		[WORD_PARAM] = { "${", "}" , PARAM_MAP},
		[WORD_SUBSHELL] = { "(", ")", SUBSHELL_MAP},
		[WORD_ARITHMETIC] = {"$((", "))" , ARITHMETIC_MAP},
		[WORD_ARITHMETIC_PAREN] = { "(", ")" , ARITHMETIC_PAREN_MAP},
		[WORD_SINGLE_QUOTE] = {"'", "'", SINGLE_QUOTE_MAP},
		[WORD_DOUBLE_QUOTE] = {"\"", "\"", DOUBLE_QUOTE_MAP},
		[WORD_PROC_SUB_IN] = {"<(", ")", PROCESS_SUB_MAP},
		[WORD_PROC_SUB_OUT] = {">(", ")", PROCESS_SUB_MAP},
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

		if (input->size >= 2 && *input->data == '\\') {
			WordContext open_ctx = da_peak_back(context_stack);
			if (open_ctx != WORD_SINGLE_QUOTE) {
				da_push(cache, da_pop_front(input));
				da_push(cache, da_pop_front(input));
				(pos->column)++; (pos->absolute)++;
				continue;
			}
		}

		const WordContext maybe_new_context = get_context(input, map, da_peak_back(context_stack));

		/*dprintf(2, "input: %s | context: %d\n", input->data, da_peak_back(context_stack));*/

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
			case '|' : da_push(cache, da_pop_front(input)); (pos->column)++; (pos->absolute)++; return true;
			case '(' : da_push(cache, da_pop_front(input)); (pos->column)++; (pos->absolute)++; return true;
			case ')' : da_push(cache, da_pop_front(input)); (pos->column)++; (pos->absolute)++; return true;
			case '<' : da_push(cache, da_pop_front(input)); (pos->column)++; (pos->absolute)++; return true;
			case '>' : da_push(cache, da_pop_front(input)); (pos->column)++; (pos->absolute)++; return true;
			case '\n': da_push(cache, da_pop_front(input)); (pos->line)++; (pos->column) = 0; (pos->absolute)++; return true;
		}
	}

	if (context_stack->size != 0) {
		const WordContext context = da_peak_back(context_stack);	
		if (context != WORD_WORD) { pretty_error(lexer, ""); return false; }
	}
	return true;
}
