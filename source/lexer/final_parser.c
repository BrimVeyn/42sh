/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:19:12 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/31 16:14:07 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
// #include "lexer.h"
#include "utils.h"
#include "libft.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

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

#include "colors.h"

void print_token_stack(const TokenStack *stack) {
	for (size_t i = 0; i < stack->size; i++) {
		const StackEntry *entry = stack->data[i];

		if (entry->is_terminal) {
			dprintf(2, "["C_LIGHT_BLUE"%zu"C_RESET"] "C_MAGENTA"%s"C_LIGHT_YELLOW": "C_TURQUOISE"%s"C_RESET"\n", i, tokenTypeStr(entry->symbol.token.type), entry->symbol.token.value);
		} else {
			dprintf(2, "["C_LIGHT_BLUE"%zu"C_RESET"] "C_LIGHT_YELLOW"%s"C_RESET"\n", i, tokenTypeStr(entry->symbol.non_terminal));
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
	while (true && ++i < 100) {
		int state = da_peak_back(stack)->state;
		TableEntry entry = parsingTable[state][token.type];
		print_token_stack(stack);
		dprintf(2, "action: %s %d\n", actionStr(entry.action), entry.value);

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

				StackEntry *reduced_entry = gc_unique(StackEntry, GC_SUBSHELL);
				switch (rule) {
					case 0: { /* program -> linebreak complete_commands linebreak */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Program;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Program].value;
						break;
					}
					case 1: { /* program -> linebreak */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Program;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Program].value;
						break;
					}
					case 2: { /* complete_commands -> complete_commands newline_list complete_command */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Complete_Commands;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Complete_Commands].value;
						break;
					}
					case 3: { /* complete_commands -> complete_command */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Complete_Commands;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Complete_Commands].value;
						break;
					}
					case 4: { /* complete_command -> list separator_op */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Complete_Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Complete_Command].value;
						break;
					}
					case 5: { /* complete_command -> list */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Complete_Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Complete_Command].value;
						break;
					}
					case 6: { /* list -> list separator_op and_or */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = List;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][List].value;
						break;
					}
					case 7: { /* list -> and_or */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = List;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][List].value;
						break;
					}
					case 8: { /* and_or -> pipeline */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = And_Or;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][And_Or].value;
						break;
					}
					case 9: { /* and_or -> and_or AND_IF linebreak pipeline */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = And_Or;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][And_Or].value;
						break;
					}
					case 10: { /* and_or -> and_or OR_IF linebreak pipeline */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = And_Or;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][And_Or].value;
						break;
					}
					case 11: { /* pipeline -> pipe_sequence */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Pipeline;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pipeline].value;
						break;
					}
					case 12: { /* pipeline -> BANG pipe_sequence */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Pipeline;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pipeline].value;
						break;
					}
					case 13: { /* pipe_sequence -> command */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Pipe_Sequence;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pipe_Sequence].value;
						break;
					}
					case 14: { /* pipe_sequence -> pipe_sequence PIPE linebreak command */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Pipe_Sequence;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pipe_Sequence].value;
						break;
					}
					case 15: { /* command -> simple_command */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Command].value;
						break;
					}
					case 16: { /* command -> compound_command */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Command].value;
						break;
					}
					case 17: { /* command -> compound_command redirect_list */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Command].value;
						break;
					}
					case 18: { /* command -> function_definition */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Command].value;
						break;
					}
					case 19: { /* compound_command -> brace_group */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Compound_Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 20: { /* compound_command -> subshell */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Compound_Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 21: { /* compound_command -> for_clause */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Compound_Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 22: { /* compound_command -> case_clause */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Compound_Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 23: { /* compound_command -> if_clause */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Compound_Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 24: { /* compound_command -> while_clause */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Compound_Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 25: { /* compound_command -> until_clause */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Compound_Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 26: { /* subshell -> LPAREN compound_list RPAREN */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Subshell;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Subshell].value;
						break;
					}
					case 27: { /* compound_list -> linebreak term */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Compound_List;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_List].value;
						break;
					}
					case 28: { /* compound_list -> linebreak term separator */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Compound_List;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_List].value;
						break;
					}
					case 29: { /* term -> term separator and_or */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Term;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Term].value;
						break;
					}
					case 30: { /* term -> and_or */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Term;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Term].value;
						break;
					}
					case 31: { /* for_clause -> FOR name do_group */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = For_Clause;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][For_Clause].value;
						break;
					}
					case 32: { /* for_clause -> FOR name sequential_sep do_group */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = For_Clause;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][For_Clause].value;
						break;
					}
					case 33: { /* for_clause -> FOR name linebreak in sequential_sep do_group */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = For_Clause;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][For_Clause].value;
						break;
					}
					case 34: { /* for_clause -> FOR name linebreak in wordlist sequential_sep do_group */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = For_Clause;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][For_Clause].value;
						break;
					}
					case 35: { /* name -> NAME */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Name;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Name].value;
						break;
					}
					case 36: { /* in -> IN */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = In;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][In].value;
						break;
					}
					case 37: { /* wordlist -> wordlist WORD */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Wordlist;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Wordlist].value;
						break;
					}
					case 38: { /* wordlist -> WORD */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Wordlist;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Wordlist].value;
						break;
					}
					case 39: { /* case_clause -> CASE WORD linebreak in linebreak case_list ESAC */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_Clause;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Clause].value;
						break;
					}
					case 40: { /* case_clause -> CASE WORD linebreak in linebreak case_list_ns ESAC */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_Clause;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Clause].value;
						break;
					}
					case 41: { /* case_clause -> CASE WORD linebreak in linebreak ESAC */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_Clause;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Clause].value;
						break;
					}
					case 42: { /* case_list_ns -> case_list case_item_ns */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_List_Ns;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_List_Ns].value;
						break;
					}
					case 43: { /* case_list_ns -> case_item_ns */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_List_Ns;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_List_Ns].value;
						break;
					}
					case 44: { /* case_list -> case_list case_item */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_List;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_List].value;
						break;
					}
					case 45: { /* case_list -> case_item */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_List;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_List].value;
						break;
					}
					case 46: { /* case_item_ns -> pattern RPAREN linebreak */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_Item_Ns;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item_Ns].value;
						break;
					}
					case 47: { /* case_item_ns -> pattern RPAREN compound_list */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_Item_Ns;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item_Ns].value;
						break;
					}
					case 48: { /* case_item_ns -> LPAREN pattern RPAREN linebreak */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_Item_Ns;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item_Ns].value;
						break;
					}
					case 49: { /* case_item_ns -> LPAREN pattern RPAREN linebreak */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_Item_Ns;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item_Ns].value;
						break;
					}
					case 50: { /* case_item_ns -> LPAREN pattern RPAREN compound_list */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_Item_Ns;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item_Ns].value;
						break;
					}
					case 51: { /* case_item -> pattern RPAREN linebreak DSEMI linebreak */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_Item;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item].value;
						break;
					}
					case 52: { /* case_item -> pattern RPAREN compound_list DSEMI linebreak */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_Item;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item].value;
						break;
					}
					case 53: { /* case_item -> LPAREN pattern RPAREN linebreak DSEMI linebreak */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_Item;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item].value;
						break;
					}
					case 54: { /* case_item -> LPAREN pattern RPAREN compound_list DSEMI linebreak */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Case_Item;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item].value;
						break;
					}
					case 55: { /* pattern -> WORD */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Pattern;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pattern].value;
						break;
					}
					case 56: { /* pattern -> pattern PIPE WORD */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Pattern;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pattern].value;
						break;
					}
					case 57: { /* if_clause -> IF compound_list THEN compound_list else_part FI */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = If_Clause;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][If_Clause].value;
						break;
					}
					case 58: { /* if_clause -> IF compound_list THEN compound_list FI */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = If_Clause;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][If_Clause].value;
						break;
					}
					case 59: { /* else_part -> ELIF compound_list THEN compound_list */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Else_Part;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Else_Part].value;
						break;
					}
					case 60: { /* else_part -> ELIF compound_list THEN compound_list else_part */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Else_Part;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Else_Part].value;
						break;
					}
					case 61: { /* else_part -> ELSE compound_list */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Else_Part;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Else_Part].value;
						break;
					}
					case 62: { /* while_clause -> WHILE compound_list do_group */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = While_Clause;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][While_Clause].value;
						break;
					}
					case 63: { /* until_clause -> UNTIL compound_list do_group */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Until_Clause;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Until_Clause].value;
						break;
					}
					case 64: { /* function_definition -> fname LPAREN RPAREN linebreak function_body */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Function_Definition;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Function_Definition].value;
						break;
					}
					case 65: { /* function_body -> compound_command */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Function_Body;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Function_Body].value;
						break;
					}
					case 66: { /* function_body -> compound_command redirect_list */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Function_Body;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Function_Body].value;
						break;
					}
					case 67: { /* fname -> NAME */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Fname;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Fname].value;
						break;
					}
					case 68: { /* brace_group -> LBRACE compound_list RBRACE */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Brace_Group;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Brace_Group].value;
						break;
					}
					case 69: { /* do_group -> DO compound_list DONE */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Do_Group;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Do_Group].value;
						break;
					}
					case 70: { /* simple_command -> cmd_prefix cmd_word cmd_suffix */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Simple_Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Simple_Command].value;
						break;
					}
					case 71: { /* simple_command -> cmd_prefix cmd_word */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Simple_Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Simple_Command].value;
						break;
					}
					case 72: { /* simple_command -> cmd_prefix */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Simple_Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Simple_Command].value;
						break;
					}
					case 73: { /* simple_command -> cmd_name cmd_suffix */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Simple_Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Simple_Command].value;
						break;
					}
					case 74: { /* simple_command -> cmd_name */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Simple_Command;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Simple_Command].value;
						break;
					}
					case 75: { /* cmd_name -> WORD */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Cmd_Name;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Name].value;
						break;
					}
					case 76: { /* cmd_word -> WORD */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Cmd_Word;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Word].value;
						break;
					}
					case 77: { /* cmd_prefix -> io_redirect */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Cmd_Prefix;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Prefix].value;
						break;
					}
					case 78: { /* cmd_prefix -> cmd_prefix io_redirect */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Cmd_Prefix;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Prefix].value;
						break;
					}
					case 79: { /* cmd_prefix -> ASSIGNMENT_WORD */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Cmd_Prefix;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Prefix].value;
						break;
					}
					case 80: { /* cmd_prefix -> cmd_prefix ASSIGNMENT_WORD */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Cmd_Prefix;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Prefix].value;
						break;
					}
					case 81: { /* cmd_suffix -> io_redirect */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Cmd_Suffix;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Suffix].value;
						break;
					}
					case 82: { /* cmd_suffix -> cmd_suffix io_redirect */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Cmd_Suffix;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Suffix].value;
						break;
					}
					case 83: { /* cmd_suffix -> WORD */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Cmd_Suffix;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Suffix].value;
						break;
					}
					case 84: { /* cmd_suffix -> cmd_suffix WORD */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Cmd_Suffix;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Suffix].value;
						break;
					}
					case 85: { /* redirect_list -> io_redirect */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Redirect_List;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Redirect_List].value;
						break;
					}
					case 86: { /* redirect_list -> redirect_list io_redirect */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Redirect_List;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Redirect_List].value;
						break;
					}
					case 87: { /* io_redirect -> io_file */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Io_Redirect;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_Redirect].value;
						break;
					}
					case 88: { /* io_redirect -> IO_NUMBER io_file */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Io_Redirect;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_Redirect].value;
						break;
					}
					case 89: { /* io_redirect -> io_here */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Io_Redirect;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_Redirect].value;
						break;
					}
					case 90: { /* io_redirect -> IO_NUMBER io_here */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Io_Redirect;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_Redirect].value;
						break;
					}
					case 91: { /* io_file -> LESS filename */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Io_File;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_File].value;
						break;
					}
					case 92: { /* io_file -> LESSAND filename */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Io_File;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_File].value;
						break;
					}
					case 93: { /* io_file -> GREAT filename */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Io_File;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_File].value;
						break;
					}
					case 94: { /* io_file -> GREATAND filename */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Io_File;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_File].value;
						break;
					}
					case 95: { /* io_file -> DGREAT filename */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Io_File;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_File].value;
						break;
					}
					case 96: { /* io_file -> LESSGREAT filename */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Io_File;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_File].value;
						break;
					}
					case 97: { /* io_file -> CLOBBER filename */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Io_File;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_File].value;
						break;
					}
					case 98: { /* filename -> WORD */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Filename;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Filename].value;
						break;
					}
					case 99: { /* io_here -> DLESS here_end */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Io_Here;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_Here].value;
						break;
					}
					case 100: { /* io_here -> DLESSDASH here_end */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Io_Here;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_Here].value;
						break;
					}
					case 101: { /* here_end -> WORD */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Here_End;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Here_End].value;
						break;
					}
					case 102: { /* newline_list -> NEWLINE */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Newline_List;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Newline_List].value;
						break;
					}
					case 103: { /* newline_list -> newline_list NEWLINE */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Newline_List;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Newline_List].value;
						break;
					}
					case 104: { /* linebreak -> newline_list */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Linebreak;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Linebreak].value;
						break;
					}
					case 105: { /* linebreak -> '' */
						// da_pop(stack);
						reduced_entry->symbol.non_terminal = Linebreak;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Linebreak].value;
						break;
					}
					case 106: { /* separator_op -> AMPER */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Separator_Op;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Separator_Op].value;
						break;
					}
					case 107: { /* separator_op -> SEMI */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Separator_Op;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Separator_Op].value;
						break;
					}
					case 108: { /* separator -> separator_op linebreak */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Separator;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Separator].value;
						break;
					}
					case 109: { /* separator -> newline_list */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Separator;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Separator].value;
						break;
					}
					case 110: { /* sequential_sep -> SEMI linebreak */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Sequential_Sep;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Sequential_Sep].value;
						break;
					}
					case 111: { /* sequential_sep -> newline_list */
						da_pop(stack);
						reduced_entry->symbol.non_terminal = Sequential_Sep;
						reduced_entry->is_terminal = false;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Sequential_Sep].value;
						break;
					}
				}
				da_push(stack, reduced_entry);
				break;
			}
			case ACCEPT: {
				return da_pop(stack);
				break;
			}
			case ERROR: {
				dprintf(2, "ERROR");
				return NULL;
				break;
			}
			case GOTO: {
				dprintf(2, "GOTO reached, should ne happen\n");
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
