/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:19:12 by nbardavi          #+#    #+#             */
/*   Updated: 2024/11/01 16:37:06 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "utils.h"
#include "libft.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
	char *io_number;
	TokenType type;
	char *filename;
} RedirectionP;

typedef struct {
	RedirectionP **data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int	gc_level;
} RedirectionL;

typedef struct {
	char **data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int	gc_level;
} StringListL;

typedef struct {
	RedirectionL *redir_list;
	StringListL	*word_list;
} SimpleCommandP;

typedef struct {
	char *value;
} FilenameP;

typedef struct {
	TokenType type;
	union {
		char *raw_value;
		RedirectionP *redir;
		FilenameP *filename;
		RedirectionL *redir_list;
		SimpleCommandP *simple_command;
	};
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

#include "colors.h"

void print_token_stack(const TokenStack *stack) {
	for (size_t i = 0; i < stack->size; i++) {
		const StackEntry *entry = stack->data[i];

		if (entry->token.type <= END) {
			dprintf(2, "["C_LIGHT_BLUE"%zu"C_RESET"] "C_MAGENTA"%s"C_LIGHT_YELLOW": "C_TURQUOISE"%s"C_RESET"\n", i, tokenTypeStr(entry->token.type), entry->token.raw_value);
		} else {
			dprintf(2, "["C_LIGHT_BLUE"%zu"C_RESET"] "C_LIGHT_YELLOW"%s "C_RESET, i, tokenTypeStr(entry->token.type));
			switch (entry->token.type) {
				case Io_Redirect:
				case Io_File: {
					dprintf(2, C_MAGENTA"{"C_LIGHT_ORANGE"%s "C_RESET"|"C_LIGHT_ORANGE" %s "C_MAGENTA"}"C_RESET"\n", tokenTypeStr(entry->token.redir->type), entry->token.redir->filename);
					break;
				}
				case Cmd_Suffix:
				case Simple_Command:
				case Cmd_Prefix: {
					dprintf(2, C_MAGENTA"{"C_LIGHT_ORANGE"%s "C_RESET"|"C_LIGHT_ORANGE" %zu "C_MAGENTA"}"C_RESET"\n", entry->token.simple_command->word_list->data[0], entry->token.simple_command->redir_list->size);
					break;
				}
				case Cmd_Name:
				case Cmd_Word: {
					dprintf(2, C_MAGENTA"["C_LIGHT_ORANGE"%s"C_MAGENTA"]"C_RESET"\n", entry->token.raw_value);
					break;
				}
				default: dprintf(2, "\n");
			}
		}
	}
}

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

TokenType identify_token(const char *raw_value, const int table_row) {
	if (!*raw_value)
		return END;

	static const char *map[] = {
		[AND_IF] = "&&", [OR_IF] = "||",
		[BANG] = "!", [PIPE] = "pipe",
		[LPAREN] = "(", [RPAREN] = ")",
		[LBRACE] = "{", [RBRACE] = "}",
		[FOR] = "for",
		[IN] = "in",
		[CASE] = "case", [ESAC] = "esac", [DSEMI] = ";;",
		[IF] = "if", [THEN] = "then", [FI] = "fi", [ELIF] = "elif", [ELSE] = "else",
		[WHILE] =  "while",
		[UNTIL] = "until",
		[DO] = "do", [DONE] = "done",
		[DLESS] = "<<", [DLESSDASH] = "<<-", [LESS] = "<", [LESSAND] = "<&",
		[DGREAT] = ">>", [GREAT] = ">", [GREATAND] = ">&",
		[LESSGREAT] = "<>", [CLOBBER] = ">|",
		[NEWLINE] = "\n", [AMPER] = "&", [SEMI] = ";",
	};
	   //NAME | WORD | IO_NUMBER | ASSIGNMENT_WORD
	
	for (size_t i = 0; i < ARRAY_SIZE(map); i++) {
		if (!ft_strcmp(map[i], raw_value) && table_row != 27) {
			return i;
		}
	}

	return WORD;
}

extern TableEntry parsingTable[182][86];

RedirectionP *createRedirFromIOFile(Tokenn *filename, Tokenn *type) {
	RedirectionP *self = gc_unique(RedirectionP, GC_SUBSHELL);
	self->type = type->type;
	self->filename = filename->filename->value;
	return self;
}

SimpleCommandP *createSimpleCommand(void) {
	SimpleCommandP *command = gc_unique(SimpleCommandP, GC_SUBSHELL);
	da_create(redir_list, RedirectionL, sizeof(RedirectionP *), GC_SUBSHELL);
	da_create(word_list, StringListL, sizeof(char *), GC_SUBSHELL);
	command->word_list = word_list;
	command->redir_list = redir_list;
	return command;
}

StackEntry *parse() {

	da_create(stack, TokenStack, sizeof(StackEntry), GC_SUBSHELL);

	Tokenn token;
	token.raw_value = lex_interface(LEX_GET, NULL);
	token.type = identify_token(token.raw_value, 0);

	int i = 0;
	while (true && ++i < 100) {
		int state = da_peak_back(stack)->state;
		TableEntry entry = parsingTable[state][token.type];
		print_token_stack(stack);
		dprintf(2, "action: %s %d\n", actionStr(entry.action), entry.value);

		switch(entry.action) {
			case SHIFT: {
				StackEntry *new_entry = gc_unique(StackEntry, GC_SUBSHELL);
				
				new_entry->token = token;
				new_entry->state = entry.value;
				da_push(stack, new_entry);
				token.raw_value = lex_interface(LEX_GET, NULL);
				token.type = identify_token(token.raw_value, entry.value);
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
						reduced_entry->token.type = Program;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Program].value;
						break;
					}
					case 1: { /* program -> linebreak */
						da_pop(stack);
						reduced_entry->token.type = Program;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Program].value;
						break;
					}
					case 2: { /* complete_commands -> complete_commands newline_list complete_command */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Complete_Commands;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Complete_Commands].value;
						break;
					}
					case 3: { /* complete_commands -> complete_command */
						da_pop(stack);
						reduced_entry->token.type = Complete_Commands;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Complete_Commands].value;
						break;
					}
					case 4: { /* complete_command -> list separator_op */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Complete_Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Complete_Command].value;
						break;
					}
					case 5: { /* complete_command -> list */
						da_pop(stack);
						reduced_entry->token.type = Complete_Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Complete_Command].value;
						break;
					}
					case 6: { /* list -> list separator_op and_or */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = List;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][List].value;
						break;
					}
					case 7: { /* list -> and_or */
						da_pop(stack);
						reduced_entry->token.type = List;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][List].value;
						break;
					}
					case 8: { /* and_or -> pipeline */
						da_pop(stack);
						reduced_entry->token.type = And_Or;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][And_Or].value;
						break;
					}
					case 9: { /* and_or -> and_or AND_IF linebreak pipeline */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = And_Or;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][And_Or].value;
						break;
					}
					case 10: { /* and_or -> and_or OR_IF linebreak pipeline */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = And_Or;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][And_Or].value;
						break;
					}
					case 11: { /* pipeline -> pipe_sequence */
						da_pop(stack);
						reduced_entry->token.type = Pipeline;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pipeline].value;
						break;
					}
					case 12: { /* pipeline -> BANG pipe_sequence */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Pipeline;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pipeline].value;
						break;
					}
					case 13: { /* pipe_sequence -> command */
						da_pop(stack);
						reduced_entry->token.type = Pipe_Sequence;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pipe_Sequence].value;
						break;
					}
					case 14: { /* pipe_sequence -> pipe_sequence PIPE linebreak command */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Pipe_Sequence;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pipe_Sequence].value;
						break;
					}
					case 15: { /* command -> simple_command */
						da_pop(stack);
						reduced_entry->token.type = Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Command].value;
						break;
					}
					case 16: { /* command -> compound_command */
						da_pop(stack);
						reduced_entry->token.type = Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Command].value;
						break;
					}
					case 17: { /* command -> compound_command redirect_list */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Command].value;
						break;
					}
					case 18: { /* command -> function_definition */
						da_pop(stack);
						reduced_entry->token.type = Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Command].value;
						break;
					}
					case 19: { /* compound_command -> brace_group */
						da_pop(stack);
						reduced_entry->token.type = Compound_Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 20: { /* compound_command -> subshell */
						da_pop(stack);
						reduced_entry->token.type = Compound_Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 21: { /* compound_command -> for_clause */
						da_pop(stack);
						reduced_entry->token.type = Compound_Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 22: { /* compound_command -> case_clause */
						da_pop(stack);
						reduced_entry->token.type = Compound_Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 23: { /* compound_command -> if_clause */
						da_pop(stack);
						reduced_entry->token.type = Compound_Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 24: { /* compound_command -> while_clause */
						da_pop(stack);
						reduced_entry->token.type = Compound_Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 25: { /* compound_command -> until_clause */
						da_pop(stack);
						reduced_entry->token.type = Compound_Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 26: { /* subshell -> LPAREN compound_list RPAREN */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Subshell;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Subshell].value;
						break;
					}
					case 27: { /* compound_list -> linebreak term */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Compound_List;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_List].value;
						break;
					}
					case 28: { /* compound_list -> linebreak term separator */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Compound_List;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_List].value;
						break;
					}
					case 29: { /* term -> term separator and_or */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Term;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Term].value;
						break;
					}
					case 30: { /* term -> and_or */
						da_pop(stack);
						reduced_entry->token.type = Term;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Term].value;
						break;
					}
					case 31: { /* for_clause -> FOR name do_group */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = For_Clause;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][For_Clause].value;
						break;
					}
					case 32: { /* for_clause -> FOR name sequential_sep do_group */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = For_Clause;
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
						reduced_entry->token.type = For_Clause;
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
						reduced_entry->token.type = For_Clause;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][For_Clause].value;
						break;
					}
					case 35: { /* name -> NAME */
						da_pop(stack);
						reduced_entry->token.type = Name;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Name].value;
						break;
					}
					case 36: { /* in -> IN */
						da_pop(stack);
						reduced_entry->token.type = In;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][In].value;
						break;
					}
					case 37: { /* wordlist -> wordlist WORD */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Wordlist;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Wordlist].value;
						break;
					}
					case 38: { /* wordlist -> WORD */
						da_pop(stack);
						reduced_entry->token.type = Wordlist;
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
						reduced_entry->token.type = Case_Clause;
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
						reduced_entry->token.type = Case_Clause;
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
						reduced_entry->token.type = Case_Clause;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Clause].value;
						break;
					}
					case 42: { /* case_list_ns -> case_list case_item_ns */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Case_List_Ns;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_List_Ns].value;
						break;
					}
					case 43: { /* case_list_ns -> case_item_ns */
						da_pop(stack);
						reduced_entry->token.type = Case_List_Ns;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_List_Ns].value;
						break;
					}
					case 44: { /* case_list -> case_list case_item */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Case_List;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_List].value;
						break;
					}
					case 45: { /* case_list -> case_item */
						da_pop(stack);
						reduced_entry->token.type = Case_List;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_List].value;
						break;
					}
					case 46: { /* case_item_ns -> pattern RPAREN linebreak */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Case_Item_Ns;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item_Ns].value;
						break;
					}
					case 47: { /* case_item_ns -> pattern RPAREN compound_list */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Case_Item_Ns;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item_Ns].value;
						break;
					}
					case 48: { /* case_item_ns -> LPAREN pattern RPAREN linebreak */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Case_Item_Ns;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item_Ns].value;
						break;
					}
					case 49: { /* case_item_ns -> LPAREN pattern RPAREN linebreak */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Case_Item_Ns;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item_Ns].value;
						break;
					}
					case 50: { /* case_item_ns -> LPAREN pattern RPAREN compound_list */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Case_Item_Ns;
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
						reduced_entry->token.type = Case_Item;
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
						reduced_entry->token.type = Case_Item;
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
						reduced_entry->token.type = Case_Item;
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
						reduced_entry->token.type = Case_Item;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item].value;
						break;
					}
					case 55: { /* pattern -> WORD */
						da_pop(stack);
						reduced_entry->token.type = Pattern;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pattern].value;
						break;
					}
					case 56: { /* pattern -> pattern PIPE WORD */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Pattern;
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
						reduced_entry->token.type = If_Clause;
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
						reduced_entry->token.type = If_Clause;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][If_Clause].value;
						break;
					}
					case 59: { /* else_part -> ELIF compound_list THEN compound_list */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Else_Part;
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
						reduced_entry->token.type = Else_Part;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Else_Part].value;
						break;
					}
					case 61: { /* else_part -> ELSE compound_list */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Else_Part;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Else_Part].value;
						break;
					}
					case 62: { /* while_clause -> WHILE compound_list do_group */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = While_Clause;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][While_Clause].value;
						break;
					}
					case 63: { /* until_clause -> UNTIL compound_list do_group */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Until_Clause;
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
						reduced_entry->token.type = Function_Definition;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Function_Definition].value;
						break;
					}
					case 65: { /* function_body -> compound_command */
						da_pop(stack);
						reduced_entry->token.type = Function_Body;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Function_Body].value;
						break;
					}
					case 66: { /* function_body -> compound_command redirect_list */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Function_Body;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Function_Body].value;
						break;
					}
					case 67: { /* fname -> NAME */
						da_pop(stack);
						reduced_entry->token.type = Fname;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Fname].value;
						break;
					}
					case 68: { /* brace_group -> LBRACE compound_list RBRACE */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Brace_Group;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Brace_Group].value;
						break;
					}
					case 69: { /* do_group -> DO compound_list DONE */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Do_Group;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Do_Group].value;
						break;
					}
					case 70: { /* simple_command -> cmd_prefix cmd_word cmd_suffix */
						da_pop(stack);
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Simple_Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Simple_Command].value;
						break;
					}
					case 71: { /* simple_command -> cmd_prefix cmd_word */
						char *word = da_pop(stack)->token.raw_value;
						SimpleCommandP *prefix = da_pop(stack)->token.simple_command;
						da_push(prefix->word_list, word);
						reduced_entry->token.type = Simple_Command;
						reduced_entry->token.simple_command = prefix;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Simple_Command].value;
						break;
					}
					case 72: { /* simple_command -> cmd_prefix */
						da_pop(stack);
						reduced_entry->token.type = Simple_Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Simple_Command].value;
						break;
					}
					case 73: { /* simple_command -> cmd_name cmd_suffix */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Simple_Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Simple_Command].value;
						break;
					}
					case 74: { /* simple_command -> cmd_name */
						char *word = da_pop(stack)->token.raw_value;
						SimpleCommandP *command = createSimpleCommand();
						da_push(command->word_list, word);
						reduced_entry->token.type = Simple_Command;
						reduced_entry->token.simple_command = command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Simple_Command].value;
						break;
					}
					case 75: { /* cmd_name -> WORD */
						char *word = da_pop(stack)->token.raw_value;
						reduced_entry->token.type = Cmd_Name;
						reduced_entry->token.raw_value = word;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Name].value;
						break;
					}
					case 76: { /* cmd_word -> WORD */
						char *word = da_pop(stack)->token.raw_value;
						reduced_entry->token.type = Cmd_Word;
						reduced_entry->token.raw_value = word;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Word].value;
						break;
					}
					case 77: { /* cmd_prefix -> io_redirect */
						RedirectionP *redir = da_pop(stack)->token.redir;
						SimpleCommandP *command = createSimpleCommand();
						da_push(command->redir_list, redir);
						reduced_entry->token.type = Cmd_Prefix;
						reduced_entry->token.simple_command = command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Prefix].value;
						break;
					}
					case 78: { /* cmd_prefix -> cmd_prefix io_redirect */
						RedirectionP *redir = da_pop(stack)->token.redir;
						SimpleCommandP *command = da_pop(stack)->token.simple_command;
            da_push(command->redir_list, redir);
						reduced_entry->token.type = Cmd_Prefix;
            reduced_entry->token.simple_command = command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Prefix].value;
						break;
					}
					case 79: { /* cmd_prefix -> ASSIGNMENT_WORD */
						da_pop(stack);
						reduced_entry->token.type = Cmd_Prefix;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Prefix].value;
						break;
					}
					case 80: { /* cmd_prefix -> cmd_prefix ASSIGNMENT_WORD */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Cmd_Prefix;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Prefix].value;
						break;
					}
					case 81: { /* cmd_suffix -> io_redirect */
						RedirectionP *redir = da_pop(stack)->token.redir;
						SimpleCommandP *command = createSimpleCommand();
						da_push(command->redir_list, redir);
						reduced_entry->token.type = Cmd_Suffix;
						reduced_entry->token.simple_command = command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Suffix].value;
						break;
					}
					case 82: { /* cmd_suffix -> cmd_suffix io_redirect */
						RedirectionP *redir = da_pop(stack)->token.redir;
						reduced_entry->token.simple_command = da_pop(stack)->token.simple_command;
						da_push(reduced_entry->token.simple_command->redir_list, redir);
						reduced_entry->token.type = Cmd_Suffix;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Suffix].value;
						break;
					}
					case 83: { /* cmd_suffix -> WORD */
						char *word = da_pop(stack)->token.raw_value;
						SimpleCommandP *command = createSimpleCommand();
						da_push(command->word_list, word);
						reduced_entry->token.type = Cmd_Suffix;
						reduced_entry->token.simple_command = command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Suffix].value;
						break;
					}
					case 84: { /* cmd_suffix -> cmd_suffix WORD */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Cmd_Suffix;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Suffix].value;
						break;
					}
					case 85: { /* redirect_list -> io_redirect */
						da_pop(stack);
						reduced_entry->token.type = Redirect_List;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Redirect_List].value;
						break;
					}
					case 86: { /* redirect_list -> redirect_list io_redirect */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Redirect_List;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Redirect_List].value;
						break;
					}
					case 89: /* io_redirect -> io_here */
					case 87: /* io_redirect -> io_file */ {
						RedirectionP *redir = da_pop(stack)->token.redir;
						reduced_entry->token.type = Io_Redirect;
						reduced_entry->token.redir = redir;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_Redirect].value;
						break;
					}
					case 88: { /* io_redirect -> IO_NUMBER io_file */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Io_Redirect;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_Redirect].value;
						break;
					}
					case 90: { /* io_redirect -> IO_NUMBER io_here */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Io_Redirect;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_Redirect].value;
						break;
					}
					case 91: /* io_file -> LESS filename */
					case 92: /* io_file -> LESSAND filename */
					case 93: /* io_file -> GREAT filename */
					case 94: /* io_file -> GREATAND filename */
					case 95: /* io_file -> DGREAT filename */
					case 96: /* io_file -> LESSGREAT filename */
					case 97: /* io_file -> CLOBBER filename */  {
						Tokenn filename = da_pop(stack)->token;
						Tokenn type = da_pop(stack)->token;
						reduced_entry->token.type = Io_File;
						reduced_entry->token.redir = createRedirFromIOFile(&filename, &type);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_File].value;
						break;
					}
					case 99: /* io_here -> DLESS here_end */ /*      TODO: start here_doc */
					case 100: { /* io_here -> DLESSDASH here_end */ //  TODO: start here_doc no expansions */
						Tokenn filename = da_pop(stack)->token;
						Tokenn type = da_pop(stack)->token;
						reduced_entry->token.type = Io_File;
						reduced_entry->token.redir = createRedirFromIOFile(&filename, &type);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_Here].value;
						break;
					}
					case 98: { /* filename -> WORD */
						Tokenn word = da_pop(stack)->token;
						reduced_entry->token.type = Filename;
						reduced_entry->token.filename = gc_unique(FilenameP, GC_SUBSHELL);
						reduced_entry->token.filename->value = word.raw_value;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Filename].value;
						break;
					}
					case 101: { /* here_end -> WORD */
						//TODO: Directly apply quote remove cuz of no expansions
						Tokenn word = da_pop(stack)->token;
						reduced_entry->token.type = Filename;
						reduced_entry->token.filename = gc_unique(FilenameP, GC_SUBSHELL);
						reduced_entry->token.filename->value = word.raw_value;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Here_End].value;
						break;
					}
					case 102: { /* newline_list -> NEWLINE */
						da_pop(stack);
						reduced_entry->token.type = Newline_List;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Newline_List].value;
						break;
					}
					case 103: { /* newline_list -> newline_list NEWLINE */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Newline_List;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Newline_List].value;
						break;
					}
					case 104: { /* linebreak -> newline_list */
						da_pop(stack);
						reduced_entry->token.type = Linebreak;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Linebreak].value;
						break;
					}
					case 105: { /* linebreak -> '' */
						// da_pop(stack);
						reduced_entry->token.type = Linebreak;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Linebreak].value;
						break;
					}
					case 106: { /* separator_op -> AMPER */
						da_pop(stack);
						reduced_entry->token.type = Separator_Op;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Separator_Op].value;
						break;
					}
					case 107: { /* separator_op -> SEMI */
						da_pop(stack);
						reduced_entry->token.type = Separator_Op;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Separator_Op].value;
						break;
					}
					case 108: { /* separator -> separator_op linebreak */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Separator;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Separator].value;
						break;
					}
					case 109: { /* separator -> newline_list */
						da_pop(stack);
						reduced_entry->token.type = Separator;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Separator].value;
						break;
					}
					case 110: { /* sequential_sep -> SEMI linebreak */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Sequential_Sep;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Sequential_Sep].value;
						break;
					}
					case 111: { /* sequential_sep -> newline_list */
						da_pop(stack);
						reduced_entry->token.type = Sequential_Sep;
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
			case GOTO: 
			case ERROR: {
				dprintf(2, C_RED"ERROR"C_RESET"\n");
				return NULL;
				break;
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