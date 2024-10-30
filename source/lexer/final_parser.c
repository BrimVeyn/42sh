/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:19:12 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/30 17:29:05 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "lexer.h"
#include "utils.h"
#include "libft.h"

#include <stdint.h>
#include <stdio.h>

typedef struct {
	TokenType type;
	char	 *value;
} Tokenn;

typedef struct {
	bool is_terminal;
	union {
		Tokenn token;
		TokenType non_terminal;
	} symbol;
	int state;
} StackEntry;

typedef struct {
	StackEntry **data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int	gc_level;
} TokenStack;

void print_token_stack(const TokenStack *stack) {
	for (size_t i = 0; i < stack->size; i++) {
		const StackEntry *entry = stack->data[i];

		if (entry->is_terminal) {
			dprintf(2, "[%zu] token->value = %s\n", i, entry->symbol.token.value);
		} else {
			dprintf(2, "[%zu] non_term: %s\n", i, tokenTypeStr(entry->symbol.non_terminal));
		}
	}
}

TokenType identify_token(const char *value) {
	if (!*value)
		return END;

	if (!ft_strcmp("&&", value)) {
		return AND_IF;
	} else if (!ft_strcmp("||", value)) {
		return OR_IF;
	} else if (!ft_strcmp("&", value)) {
		return AMPER;
	} else if (!ft_strcmp(";", value)) {
		return SEMI;
	} else {
		return WORD;
	}
}

extern TableEntry parsingTable[182][86];

StackEntry *parse() {

	da_create(stack, TokenStack, sizeof(StackEntry), GC_SUBSHELL);

	Tokenn token;
	token.value = lex_interface(LEX_GET, NULL);
	token.type = identify_token(token.value);

	int i = 0;
	while (i++ < 10) {
		int state = da_peak_back(stack)->state;
		TableEntry entry = parsingTable[state][token.type];
		print_token_stack(stack);
		dprintf(2, "state: %d\n", state);
		dprintf(2, "action: %s\n", actionStr(entry.action));

		switch(entry.action) {
			case SHIFT: {
				StackEntry *new_entry = gc_unique(StackEntry, GC_SUBSHELL);
				new_entry->is_terminal = true;
				new_entry->symbol.token = token;
				new_entry->state = entry.value;
				da_push(stack, new_entry);
				token.value = lex_interface(LEX_GET, NULL);
				token.type = identify_token(token.value);
				break;
			}
			case REDUCE: {
				int rule = entry.value;
				dprintf(2, "rule: %d\n", rule);
				StackEntry *reduced_entry = gc_unique(StackEntry, GC_SUBSHELL);
				switch (rule) {
					case 1: { //R1 ==> program -> linebrea complete_commands linebreak
						da_pop(stack);
						reduced_entry->symbol.non_terminal = List;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state; // État après réduction
						reduced_entry->state = parsingTable[state][In].value;
						break;
					}
					da_push(stack, reduced_entry);
					break;
				}
				break;
			}
			case ACCEPT: {
				return da_pop(stack);
				break;
			}
			case ERROR: {
				dprintf(2, "ERROR fdp\n");
				return NULL;
				break;
			}
			case GOTO: {
				return NULL;
			}
		}
	}
	return NULL;
}


void parse_input(char *in) {
	lex_interface(LEX_SET, in);
	// for (size_t i = 0; i < 10; i++) {
	// 	char *value = lex_interface(LEX_GET, NULL);	
	// 	TokenType type = identify_token(value);
	// 	dprintf(2, "|%s| -> %s\n", value, tokenTypeStr(type));
	// }
	parse();
}
