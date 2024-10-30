/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_get_next_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:19:12 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/30 16:59:41 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "lexer.h"
// #include "parser.h"
// #include "utils.h"
// #include "libft.h"
// #include "debug.h"
// #include <stdint.h>
// #include <stdio.h>
//
//
// typedef enum { 
// 	AND_IF, 
// 	OR_IF, 
// 	AMPER, 
// 	SEMI, 
// 	WORD, 
// 	END 
// } TokenType;
//
// typedef enum { SHIFT, REDUCE, ACCEPT, ERROR } Action;
//
// typedef struct {
// 	Action action;
// 	int value;
// } TableEntry;
//
// typedef enum { 
// 	LIST, 
// 	AND_OR, 
// 	SEPARATOR, 
// 	COMMAND, 
// 	WORD_LIST 
// } NonTerminal;
//
// typedef struct {
// 	TokenType type;
// 	char	 *value;
// } Tokenn;
//
// typedef struct {
// 	bool is_terminal;
// 	union {
// 		Tokenn token;
// 		NonTerminal non_terminal;
// 	} symbol;
// 	int state;
// } StackEntry;
//
// typedef struct {
// 	StackEntry **data;
// 	size_t size;
// 	size_t capacity;
// 	size_t size_of_element;
// 	int	gc_level;
// } TokenStack;
//
// TokenType identify_token(const char *value) {
// 	if (!*value)
// 		return END;
//
// 	if (!ft_strcmp("&&", value)) {
// 		return AND_IF;
// 	} else if (!ft_strcmp("||", value)) {
// 		return OR_IF;
// 	} else if (!ft_strcmp("&", value)) {
// 		return AMPER;
// 	} else if (!ft_strcmp(";", value)) {
// 		return SEMI;
// 	} else {
// 		return WORD;
// 	}
// }
//
// char *nonTermStr(const NonTerminal item) {
// 	switch (item) {
// 		case LIST: return "LIST";
// 		case AND_OR: return "AND_OR";
// 		case WORD_LIST: return "WORD_LIST";
// 		case SEPARATOR: return "SEPARATOR";
// 		case COMMAND: return "COMMAND";
// 	}
// 	return NULL;
// }
//
// char *actionStr(const Action action) {
// 	switch (action) {
// 		case ERROR: return "ERROR";
// 		case REDUCE: return "REDUCE";
// 		case SHIFT: return "SHIFT";
// 		case ACCEPT: return "ACCEPT";
// 	}
// 	return NULL;
// }
//
// char *tokenTypeStr(const TokenType type) {
// 	switch (type) {
// 		case END: return "END";
// 		case WORD: return "WORD";
// 		case AND_IF: return "AND_IF";
// 		case OR_IF: return "OR_IF";
// 		case AMPER: return "AMPER";
// 		case SEMI: return "SEMI";
// 	}
// 	return NULL;
// }
//
// void print_token_stack(const TokenStack *stack) {
// 	for (size_t i = 0; i < stack->size; i++) {
// 		const StackEntry *entry = stack->data[i];
//
// 		if (entry->is_terminal) {
// 			dprintf(2, "[%zu] token->value = %s\n", i, entry->symbol.token.value);
// 		} else {
// 			dprintf(2, "[%zu] non_term: %s\n", i, nonTermStr(entry->symbol.non_terminal));
// 		}
// 	}
// }
//
// #include "colors.h"
//
// StackEntry *parse() {
//
// 	static TableEntry parsingTable[15][6] = {
// 		/*STATE    AND_IF       OR_IF        AMPER      SEMI        WORD         END     */
// 		/* 0 */ { {ERROR, 0}, {ERROR, 0}, {ERROR, 0}, {ERROR, 0}, {SHIFT, 5}, {ERROR, 0} },
// 		/* 1 */ { {ERROR, 0}, {ERROR, 0}, {SHIFT, 7}, {SHIFT, 8}, {ERROR, 0}, {ACCEPT, 0} },
// 		/* 2 */ { {SHIFT, 9}, {SHIFT, 10}, {REDUCE, 1}, {REDUCE, 1}, {ERROR, 0}, {REDUCE, 1} },
// 		/* 3 */ { {REDUCE, 4}, {REDUCE, 4}, {REDUCE, 4}, {REDUCE, 4}, {ERROR, 0}, {REDUCE, 4} },
// 		/* 4 */ { {REDUCE, 9}, {REDUCE, 9}, {REDUCE, 9}, {REDUCE, 9}, {SHIFT, 11}, {REDUCE, 9} },
// 		/* 5 */ { {REDUCE, 10}, {REDUCE, 10}, {REDUCE, 10}, {REDUCE, 10}, {REDUCE, 10}, {REDUCE, 10} },
// 		/* 6 */ { {ERROR, 0}, {ERROR, 0}, {REDUCE, 3}, {REDUCE, 3}, {SHIFT, 5}, {REDUCE, 3} },
// 		/* 7 */ { {ERROR, 0}, {ERROR, 0}, {REDUCE, 7}, {REDUCE, 7}, {REDUCE, 7}, {REDUCE, 7} },
// 		/* 8 */ { {ERROR, 0}, {ERROR, 0}, {REDUCE, 8}, {REDUCE, 8}, {REDUCE, 8}, {REDUCE, 8} },
// 		/* 9 */ { {ERROR, 0}, {ERROR, 0}, {ERROR, 0}, {ERROR, 0}, {SHIFT, 5}, {ERROR, 0} },
// 		/*10 */ { {ERROR, 0}, {ERROR, 0}, {ERROR, 0}, {ERROR, 0}, {SHIFT, 5}, {ERROR, 0} },
// 		/*11 */ { {REDUCE, 11}, {REDUCE, 11}, {REDUCE, 11}, {REDUCE, 11}, {REDUCE, 11}, {REDUCE, 11} },
// 		/*12 */ { {SHIFT, 9}, {SHIFT, 10}, {REDUCE, 2}, {REDUCE, 2}, {ERROR, 0}, {REDUCE, 2} },
// 		/*13 */ { {REDUCE, 5}, {REDUCE, 5}, {REDUCE, 5}, {REDUCE, 5}, {ERROR, 0}, {REDUCE, 5} },
// 		/*14 */ { {REDUCE, 6}, {REDUCE, 6}, {REDUCE, 6}, {REDUCE, 6}, {ERROR, 0}, {REDUCE, 6} },
// 	};
//
// 	static int gotoTable[15][5] = {
// 		/*       LIST     AND_OR   SEPARATOR  COMMMAND WORD_LIST */
// 		/* 0 */ { 1,        2,       -1,        3,        4},
// 		/* 1 */ {-1,       -1,        6,       -1,       -1},
// 		/* 2 */ {-1,       -1,       -1,       -1,       -1},
// 		/* 3 */ {-1,       -1,       -1,       -1,       -1},
// 		/* 4 */ {-1,       -1,       -1,       -1,       -1},
// 		/* 5 */ {-1,       -1,       -1,       -1,       -1},
// 		/* 6 */ {-1,       12,       -1,        3,        4},
// 		/* 7 */ {-1,       -1,       -1,       -1,       -1},
// 		/* 8 */ {-1,       -1,       -1,       -1,       -1},
// 		/* 9 */ {-1,       -1,       -1,       13,        4},
// 		/*10 */ {-1,       -1,       -1,       14,        4},
// 		/*11 */ {-1,       -1,       -1,       -1,       -1},
// 		/*12 */ {-1,       -1,       -1,       -1,       -1},
// 		/*13 */ {-1,       -1,       -1,       -1,       -1},
// 		/*14 */ {-1,       -1,       -1,       -1,       -1},
// 	};
// 	
// 	da_create(stack, TokenStack, sizeof(StackEntry), GC_SUBSHELL);
//
// 	Tokenn token;
// 	token.value = lex_interface(LEX_GET, NULL);
// 	token.type = identify_token(token.value);
//
// 	while (true) {
// 		int state = da_peak_back(stack)->state;
// 		TableEntry entry = parsingTable[state][token.type];
// 		print_token_stack(stack);
// 		dprintf(2, "state: %d\n", state);
// 		dprintf(2, "action: %s\n", actionStr(entry.action));
//
// 		switch(entry.action) {
// 			case SHIFT: {
// 				StackEntry *new_entry = gc_unique(StackEntry, GC_SUBSHELL);
// 				new_entry->is_terminal = true;
// 				new_entry->symbol.token = token;
// 				new_entry->state = entry.value;
// 				da_push(stack, new_entry);
// 				token.value = lex_interface(LEX_GET, NULL);
// 				token.type = identify_token(token.value);
// 				break;
// 			}
// 			case REDUCE: {
// 				int rule = entry.value;
// 				dprintf(2, "rule: %d\n", rule);
// 				StackEntry *reduced_entry = gc_unique(StackEntry, GC_SUBSHELL);
// 				switch (rule) {
// 					case 1: {
// 						da_pop(stack);
// 						reduced_entry->symbol.non_terminal = LIST;
// 						reduced_entry->is_terminal = false;
// 						state = da_peak_back(stack)->state; // État après réduction
// 						reduced_entry->state = gotoTable[state][LIST];
// 						break;
// 					}
// 					case 2: {
// 						da_pop(stack);
// 						da_pop(stack);
// 						da_pop(stack);
// 						state = da_peak_back(stack)->state; // État après réduction
// 						reduced_entry->symbol.non_terminal = LIST;
// 						reduced_entry->is_terminal = false;
// 						reduced_entry->state = gotoTable[state][LIST];
// 						break;
// 					}
// 					case 3: {
// 						da_pop(stack);
// 						da_pop(stack);
// 						reduced_entry->symbol.non_terminal = LIST;
// 						reduced_entry->is_terminal = false;
// 						state = da_peak_back(stack)->state; // État après réduction
// 						reduced_entry->state = gotoTable[state][LIST];
// 						break;
// 					}
// 					case 4: {
// 						da_pop(stack);
// 						reduced_entry->symbol.non_terminal = AND_OR;
// 						reduced_entry->is_terminal = false;
// 						state = da_peak_back(stack)->state; // État après réduction
// 						reduced_entry->state = gotoTable[state][AND_OR];
// 						break;
// 					}
// 					case 5: {
// 						da_pop(stack);
// 						da_pop(stack);
// 						da_pop(stack);
// 						reduced_entry->symbol.non_terminal = AND_OR;
// 						reduced_entry->is_terminal = false;
// 						state = da_peak_back(stack)->state; // État après réduction
// 						reduced_entry->state = gotoTable[state][AND_OR];
// 						break;
// 					}
// 					case 6: {
// 						da_pop(stack);
// 						da_pop(stack);
// 						da_pop(stack);
// 						reduced_entry->symbol.non_terminal = AND_OR;
// 						reduced_entry->is_terminal = false;
// 						state = da_peak_back(stack)->state; // État après réduction
// 						reduced_entry->state = gotoTable[state][AND_OR];
// 						break;
// 					}
// 					case 7: {
// 						da_pop(stack);
// 						reduced_entry->symbol.non_terminal = SEPARATOR;
// 						reduced_entry->is_terminal = false;
// 						state = da_peak_back(stack)->state; // État après réduction
// 						reduced_entry->state = gotoTable[state][SEPARATOR];
// 						break;
// 					}
// 					case 8: {
// 						da_pop(stack);
// 						reduced_entry->symbol.non_terminal = SEPARATOR;
// 						reduced_entry->is_terminal = false;
// 						state = da_peak_back(stack)->state; // État après réduction
// 						reduced_entry->state = gotoTable[state][SEPARATOR];
// 						break;
// 					}
// 					case 9: {
// 						da_pop(stack);
// 						reduced_entry->symbol.non_terminal = COMMAND;
// 						reduced_entry->is_terminal = false;
// 						state = da_peak_back(stack)->state; // État après réduction
// 						reduced_entry->state = gotoTable[state][COMMAND];
// 						break;
// 					}
// 					case 10: {
// 						da_pop(stack);
// 						reduced_entry->symbol.non_terminal = WORD_LIST;
// 						reduced_entry->is_terminal = false;
// 						state = da_peak_back(stack)->state; // État après réduction
// 						reduced_entry->state = gotoTable[state][WORD_LIST];
// 						break;
// 					}
// 					case 11: {
// 						da_pop(stack);
// 						da_pop(stack);
// 						reduced_entry->symbol.non_terminal = WORD_LIST;
// 						reduced_entry->is_terminal = false;
// 						state = da_peak_back(stack)->state; // État après réduction
// 						reduced_entry->state = gotoTable[state][WORD_LIST];
// 						break;
// 					}
// 				}
// 				da_push(stack, reduced_entry);
// 				break;
// 			}
// 			case ACCEPT: {
// 				return da_pop(stack);
// 				break;
// 			}
// 			case ERROR: {
// 				dprintf(2, "ERROR fdp\n");
// 				return NULL;
// 				break;
// 			}
// 		}
// 	}
// 	return NULL;
// }
//
//
// void parse_input(char *in) {
// 	lex_interface(LEX_SET, in);
// 	// for (size_t i = 0; i < 10; i++) {
// 	// 	char *value = lex_interface(LEX_GET, NULL);	
// 	// 	TokenType type = identify_token(value);
// 	// 	dprintf(2, "|%s| -> %s\n", value, tokenTypeStr(type));
// 	// }
// 	parse();
// }
