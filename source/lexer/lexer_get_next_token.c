/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_get_next_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:19:12 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/29 17:28:55 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "parser.h"
#include "utils.h"
#include "libft.h"
#include "debug.h"
#include <stdint.h>
#include <stdio.h>

typedef enum {LEX_SET, LEX_GET, LEX_PEAK, LEX_DEBUG} LexMode;

void get_next_token(StringStream *input, StringStream *cache, size_t *line, size_t *column);

typedef struct {
	StringStream *input;
	StringStream *peak;
	size_t line; 
	size_t column;
} Lex;

char *lex_interface(LexMode mode, void *input) {
	static Lex *lexer = NULL;

	if (mode == LEX_SET) {
		lexer = gc_unique(Lex, GC_SUBSHELL);
		da_create(tmp, StringStream, sizeof(char), GC_SUBSHELL);
		lexer->input = tmp;
		da_create(tmp2, StringStream, sizeof(char), GC_SUBSHELL);
		lexer->peak = tmp;
		ss_push_string(lexer->input, input);
	} else if (mode == LEX_GET) {
		if (lexer->peak->size) {
			return ss_get_owned_slice(lexer->peak);
		} else {
			da_create(tmp, StringStream, sizeof(char), GC_SUBSHELL);
			get_next_token(lexer->input, tmp, &lexer->line, &lexer->column);
			return tmp->data;
		}
	} else if (mode == LEX_PEAK) {
		if (lexer->peak->size) {
			return lexer->peak->data;
		} else {
			get_next_token(lexer->input, lexer->peak, &lexer->line, &lexer->column);
			return lexer->peak->data;
		}
	} else if (mode == LEX_DEBUG) {
		dprintf(2, "input: %s\n", lexer->input->data);
		dprintf(2, "peak: %s\n", lexer->peak->data);
	}
	return NULL;
}

typedef enum { AND_IF, OR_IF, AMPER, SEMI, WORD, END } TokenType;

typedef enum { SHIFT, REDUCE, ACCEPT, ERROR } Action;

typedef struct {
	Action action;
	int value;
} TableEntry;

typedef enum { LIST, AND_OR, SEPARATOR, COMMAND, WORD_LIST } NonTerminal;

typedef struct {
	TokenType type;
	char	 *value;
} Tokenn;

typedef struct {
	Tokenn token;
	int state;
} StackEntry;

typedef struct {
	StackEntry **data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int	gc_level;
} TokenStack;

TokenType identify_token(const char *value) {
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

StackEntry *parse(const char *expr) {
	lex_interface(LEX_SET, (void *)expr);

	static TableEntry parsingTable[15][6] = {
		/*STATE    AND_IF       OR_IF        AMPER      SEMI        WORD         END     */
		/* 0 */ { {ERROR, 0}, {ERROR, 0}, {ERROR, 0}, {ERROR, 0}, {SHIFT, 5}, {ERROR, 0} },
		/* 1 */ { {ERROR, 0}, {ERROR, 0}, {SHIFT, 7}, {SHIFT, 8}, {ERROR, 0}, {ACCEPT, 0} },
		/* 2 */ { {SHIFT, 9}, {SHIFT, 10}, {REDUCE, 1}, {REDUCE, 1}, {ERROR, 0}, {REDUCE, 1} },
		/* 3 */ { {REDUCE, 4}, {REDUCE, 4}, {REDUCE, 4}, {REDUCE, 4}, {ERROR, 0}, {REDUCE, 4} },
		/* 4 */ { {REDUCE, 9}, {REDUCE, 9}, {REDUCE, 9}, {REDUCE, 9}, {SHIFT, 11}, {REDUCE, 9} },
		/* 5 */ { {REDUCE, 10}, {REDUCE, 10}, {REDUCE, 10}, {REDUCE, 10}, {REDUCE, 10}, {REDUCE, 10} },
		/* 6 */ { {ERROR, 0}, {ERROR, 0}, {REDUCE, 3}, {REDUCE, 3}, {SHIFT, 5}, {REDUCE, 3} },
		/* 7 */ { {ERROR, 0}, {ERROR, 0}, {REDUCE, 7}, {REDUCE, 7}, {REDUCE, 7}, {REDUCE, 7} },
		/* 8 */ { {ERROR, 0}, {ERROR, 0}, {REDUCE, 8}, {REDUCE, 8}, {REDUCE, 8}, {REDUCE, 8} },
		/* 9 */ { {ERROR, 0}, {ERROR, 0}, {ERROR, 0}, {ERROR, 0}, {SHIFT, 5}, {ERROR, 0} },
		/*10 */ { {ERROR, 0}, {ERROR, 0}, {ERROR, 0}, {ERROR, 0}, {SHIFT, 5}, {ERROR, 0} },
		/*11 */ { {REDUCE, 11}, {REDUCE, 11}, {REDUCE, 11}, {REDUCE, 11}, {REDUCE, 11}, {REDUCE, 11} },
		/*12 */ { {SHIFT, 9}, {SHIFT, 10}, {REDUCE, 2}, {REDUCE, 2}, {ERROR, 0}, {REDUCE, 2} },
		/*13 */ { {REDUCE, 5}, {REDUCE, 5}, {REDUCE, 5}, {REDUCE, 5}, {ERROR, 0}, {REDUCE, 5} },
		/*14 */ { {REDUCE, 6}, {REDUCE, 6}, {REDUCE, 6}, {REDUCE, 6}, {ERROR, 0}, {REDUCE, 6} },
	};

	static int gotoTable[15][5] = {
		/*       LIST     AND_OR   SEPARATOR  COMMMAND WORD_LIST */
		/* 0 */ { 1,        2,       -1,        3,        4},
		/* 1 */ {-1,       -1,        6,       -1,       -1},
		/* 2 */ {-1,       -1,       -1,       -1,       -1},
		/* 3 */ {-1,       -1,       -1,       -1,       -1},
		/* 4 */ {-1,       -1,       -1,       -1,       -1},
		/* 5 */ {-1,       -1,       -1,       -1,       -1},
		/* 6 */ {-1,       12,       -1,        3,        4},
		/* 7 */ {-1,       -1,       -1,       -1,       -1},
		/* 8 */ {-1,       -1,       -1,       -1,       -1},
		/* 9 */ {-1,       -1,       -1,       13,        4},
		/*10 */ {-1,       -1,       -1,       14,        4},
		/*11 */ {-1,       -1,       -1,       -1,       -1},
		/*12 */ {-1,       -1,       -1,       -1,       -1},
		/*13 */ {-1,       -1,       -1,       -1,       -1},
		/*14 */ {-1,       -1,       -1,       -1,       -1},
	};
	
	da_create(stack, TokenStack, sizeof(StackEntry), GC_SUBSHELL);

	Tokenn token;
	token.value = lex_interface(LEX_GET, NULL);
	token.type = identify_token(token.value);

	while (true) {
		int state = da_peak_back(stack)->state;
		TableEntry entry = parsingTable[state][token.type];

		switch(entry.action) {
			case SHIFT: {
				StackEntry *new_entry = gc_unique(StackEntry, GC_SUBSHELL);
				new_entry->token = token;
				new_entry->state = entry.value;
				da_push(stack, new_entry);
				token.value = lex_interface(LEX_GET, NULL);
				token.type = identify_token(token.value);
				break;
			}
			case REDUCE: {
				int rule = entry.value;
				StackEntry *new_entry = gc_unique(StackEntry, GC_SUBSHELL);
				switch (rule) {
					case 1: {
						da_pop(stack);
					}
				}
			}
			case ACCEPT: {
				return da_pop(stack);
				break;
			}
			case ERROR: {
				break;
			}
		}


	}
}


void parse_input(char *in) {
	parse(in);
}

