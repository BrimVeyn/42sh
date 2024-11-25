/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 11:38:17 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/25 11:38:18 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "ft_regex.h"
#include "utils.h"
#include "libft.h"
#include "parser.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "colors.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

extern TableEntry parsingTable[182][86];

bool is_keyword(const TokenType type) {
	return (type == IF || type == FI || type == CASE || type == ESAC ||
		 type == FOR || type == IN || type == WHILE || type == UNTIL ||
		 type == DONE || type == DO || type == LBRACE || type == RBRACE || type == THEN);
}

TokenType identify_token(const char *raw_value, const int table_row, bool *error) {
	if (!*raw_value)
		return END;

	static const char *map[] = {
		[AND_IF] = "&&", [OR_IF] = "||",
		[BANG] = "!", [PIPE] = "|",
		[LPAREN] = "(", [RPAREN] = ")",
		[LBRACE] = "{", [RBRACE] = "}",
		[FOR] = "for", [IN] = "in",
		[CASE] = "case", [ESAC] = "esac", [DSEMI] = ";;",
		[IF] = "if", [THEN] = "then", [FI] = "fi", [ELIF] = "elif", [ELSE] = "else",
		[WHILE] =  "while", [UNTIL] = "until",
		[DO] = "do", [DONE] = "done",
		[DLESS] = "<<", [DLESSDASH] = "<<-", [LESS] = "<", [LESSAND] = "<&",
		[DGREAT] = ">>", [GREAT] = ">", [GREATAND] = ">&",
		[LESSGREAT] = "<>", [CLOBBER] = ">|",
		[NEWLINE] = "\n", [AMPER] = "&", [SEMI] = ";",
	};

	// dprintf(2, "value: %s, table_row: %d\n", raw_value, table_row);
	for (size_t i = 0; i < ARRAY_SIZE(map); i++) {
		if (!ft_strcmp(map[i], raw_value) && 
			((table_row != 27 && table_row != 66) || !is_keyword(i))) {
			return i;
		}
	}

	if (regex_match("^[0-9]+$", (char *)raw_value).is_found) {
		char *peak = lex_interface(LEX_PEAK_CHAR, NULL, NULL, error);
		if (peak && (*peak == '<' || *peak == '>'))
			return IO_NUMBER;
	}

	if (regex_match("^[a-zA-Z_][a-zA-Z0-9_]*=", (char *)raw_value).is_found) {
		if (table_row != 66 && table_row != 27)
			return ASSIGNMENT_WORD;
	}

	// dprintf(2, "value: %s, table_row: %d\n", raw_value, table_row);
	if (regex_match("^[a-zA-Z_][a-zA-Z0-9_]*$", (char *)raw_value).is_found) {
		if (table_row == 30 || 
			((table_row == 48 || table_row == 0) && !ft_strcmp("(", lex_interface(LEX_PEAK, NULL, NULL, error))))
			return NAME;
	}

	return WORD;
}

#include "exec.h"

StackEntry *parse(Vars *shell_vars) {

	bool error = false;
	da_create(stack, TokenStack, sizeof(StackEntry), GC_SUBSHELL);

	Tokenn token;
	token.raw_value = lex_interface(LEX_GET, NULL, NULL, &error);
	if (error) return NULL;
	token.type = identify_token(token.raw_value, 0, &error);
	if (error) return NULL;

	size_t action_no = 0;
	while (true && ++action_no < 2000) {
		int state = da_peak_back(stack)->state;
		TableEntry entry = parsingTable[state][token.type];
		// print_token_stack(stack);
		// dprintf(2, C_DARK_GRAY_FG C_LIGHT_RED"[%zu] %s %d"C_RESET"\n", action_no, actionStr(entry.action), entry.value);

		switch(entry.action) {
			case SHIFT: {
				StackEntry *new_entry = gc_unique(StackEntry, GC_SUBSHELL);
				
				new_entry->token = token;
				new_entry->state = entry.value;
				da_push(stack, new_entry);
				token.raw_value = lex_interface(LEX_GET, NULL, NULL, &error);
				if (error) return NULL;
				token.type = identify_token(token.raw_value, entry.value, &error);
				if (error) return NULL;
				// dprintf(2, "Token produced: %s, type: %s\n", token.raw_value, tokenTypeStr(token.type));
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
						CompleteCommandP *complete_command = da_pop(stack)->token.complete_command;
						da_pop(stack); //newline_list
						da_pop(stack); //complete_commands
						// dprintf(2, "ONE COMMAND !\n");
						// print_complete_command(complete_command);
						execute_complete_command(complete_command, shell_vars);
						reduced_entry->token.type = Complete_Commands;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Complete_Commands].value;
						break;
					}
					case 3: { /* complete_commands -> complete_command */
						CompleteCommandP *complete_command = da_pop(stack)->token.complete_command;
						// dprintf(2, "ONE COMMAND !\n");
						// print_complete_command(complete_command);
						execute_complete_command(complete_command, shell_vars);
						reduced_entry->token.type = Complete_Commands;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Complete_Commands].value;
						break;
					}
					case 4: { /* complete_command -> list separator_op */
						TokenType separator = da_pop(stack)->token.separator_op;
						ListP *list = da_pop(stack)->token.list;
						reduced_entry->token.type = Complete_Command;
						reduced_entry->token.complete_command = gc_unique(CompleteCommandP, GC_SUBSHELL);
						reduced_entry->token.complete_command->list = list;
						reduced_entry->token.complete_command->separator = separator;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Complete_Command].value;
						break;
					}
					case 5: { /* complete_command -> list */
						ListP *list = da_pop(stack)->token.list;
						reduced_entry->token.type = Complete_Command;
						reduced_entry->token.complete_command = gc_unique(CompleteCommandP, GC_SUBSHELL);
						reduced_entry->token.complete_command->list = list;
						reduced_entry->token.complete_command->separator = END;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Complete_Command].value;
						break;
					}
					case 6: { /* list -> list separator_op and_or */
						AndOrP *and_or = da_pop(stack)->token.and_or;
						TokenType sep = da_pop(stack)->token.separator_op;
						ListP *list = da_pop(stack)->token.list;
						list->separator = sep;
						listAddBack(&list, listNew(and_or, END));
						reduced_entry->token.type = List;
						reduced_entry->token.list = list;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][List].value;
						break;
					}
					case 7: { /* list -> and_or */
						AndOrP *and_or = da_pop(stack)->token.and_or;
						reduced_entry->token.list = listNew(and_or, END);
						reduced_entry->token.type = List;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][List].value;
						break;
					}
					case 8: { /* and_or -> pipeline */
						PipeLineP *pipeline = da_pop(stack)->token.pipeline;
						reduced_entry->token.and_or = andOrNew(pipeline, END);
						reduced_entry->token.type = And_Or;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][And_Or].value;
						break;
					}
					case 9: /* and_or -> and_or AND_IF linebreak pipeline */
					case 10: { /* and_or -> and_or OR_IF linebreak pipeline */
						PipeLineP *pipeline = da_pop(stack)->token.pipeline;
						da_pop(stack);
						TokenType separator = da_pop(stack)->token.type;
						AndOrP *and_or = da_pop(stack)->token.and_or;
						andOrAddBack(&and_or, andOrNew(pipeline, END), separator);
						reduced_entry->token.type = And_Or;
						reduced_entry->token.and_or = and_or;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][And_Or].value;
						break;
					}
					case 11: { /* pipeline -> pipe_sequence */
						PipeLineP *pipe_sequence = da_pop(stack)->token.pipeline;
						reduced_entry->token.type = Pipeline;
						reduced_entry->token.pipeline = pipe_sequence;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pipeline].value;
						break;
					}
					case 12: { /* pipeline -> BANG pipe_sequence */
						//FIX: !
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Pipeline;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pipeline].value;
						break;
					}
					case 13: { /* pipe_sequence -> command */
						CommandP *command = da_pop(stack)->token.command;
						reduced_entry->token.type = Pipe_Sequence;
						reduced_entry->token.pipeline = pipelineNew(command);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pipe_Sequence].value;
						break;
					}
					case 14: { /* pipe_sequence -> pipe_sequence PIPE linebreak command */
						CommandP *command = da_pop(stack)->token.command;
						da_pop(stack);
						da_pop(stack);
						PipeLineP *pipe_sequence = da_pop(stack)->token.pipeline;
						pipelineAddBack(&pipe_sequence, pipelineNew(command));
						reduced_entry->token.type = Pipe_Sequence;
						reduced_entry->token.pipeline = pipe_sequence;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pipe_Sequence].value;
						break;
					}
					case 15: { /* command -> simple_command */
						SimpleCommandP *simple_command = da_pop(stack)->token.simple_command;
						reduced_entry->token.command = gc_unique(CommandP, GC_SUBSHELL);
						reduced_entry->token.command->type = Simple_Command;
						reduced_entry->token.command->simple_command = simple_command;
						reduced_entry->token.type = Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Command].value;
						break;
					}
					case 16: { /* command -> compound_command */
						CommandP *command = da_pop(stack)->token.command;
						reduced_entry->token.type = Command;
						reduced_entry->token.command = command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Command].value;
						break;
					}
					case 17: { /* command -> compound_command redirect_list */
						RedirectionL *redir_list =da_pop(stack)->token.redir_list;
						CommandP *compound_command = da_pop(stack)->token.command;
						compound_command->redir_list = redir_list;
						reduced_entry->token.type = Command;
						reduced_entry->token.command = compound_command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Command].value;
						break;
					}
					case 18: { /* command -> function_definition */
						CommandP *function_definition = da_pop(stack)->token.command;
						reduced_entry->token.type = Command;
						reduced_entry->token.command = function_definition;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Command].value;
						break;
					}
					case 19: { /* compound_command -> brace_group */
						ListP *brace_group = da_pop(stack)->token.list;
						reduced_entry->token.type = Compound_Command;
						reduced_entry->token.command = gc_unique(CommandP, GC_SUBSHELL);
						reduced_entry->token.command->brace_group = brace_group;
						reduced_entry->token.command->type = Brace_Group;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 20: { /* compound_command -> subshell */
						ListP *subshell = da_pop(stack)->token.list;
						reduced_entry->token.type = Compound_Command;
						reduced_entry->token.command = gc_unique(CommandP, GC_SUBSHELL);
						reduced_entry->token.command->subshell = subshell;
						reduced_entry->token.command->type = Subshell;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 21: { /* compound_command -> for_clause */
						ForClauseP *for_clause = da_pop(stack)->token.for_clause;
						reduced_entry->token.type = Compound_Command;
						reduced_entry->token.command = gc_unique(CommandP, GC_SUBSHELL);
						reduced_entry->token.command->for_clause = for_clause;
						reduced_entry->token.command->type = For_Clause;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 22: { /* compound_command -> case_clause */
						CaseClauseP *case_clause = da_pop(stack)->token.case_clause;
						reduced_entry->token.type = Compound_Command;
						reduced_entry->token.command = gc_unique(CommandP, GC_SUBSHELL);
						reduced_entry->token.command->type = Case_Clause;
						reduced_entry->token.command->case_clause = case_clause;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 23: { /* compound_command -> if_clause */
						IFClauseP *if_clause = da_pop(stack)->token.if_clause;
						reduced_entry->token.command = gc_unique(CommandP, GC_SUBSHELL);
						reduced_entry->token.command->type = If_Clause;
						reduced_entry->token.command->if_clause = if_clause;
						reduced_entry->token.type = Compound_Command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 24: { /* compound_command -> while_clause */
						WhileClauseP *while_clause = da_pop(stack)->token.while_clause;
						reduced_entry->token.type = Compound_Command;
						reduced_entry->token.command = gc_unique(CommandP, GC_SUBSHELL);
						reduced_entry->token.command->type = While_Clause;
						reduced_entry->token.command->while_clause = while_clause;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 25: { /* compound_command -> until_clause */
						WhileClauseP *until_clause = da_pop(stack)->token.while_clause;
						reduced_entry->token.type = Compound_Command;
						reduced_entry->token.command = gc_unique(CommandP, GC_SUBSHELL);
						reduced_entry->token.command->type = Until_Clause;
						reduced_entry->token.command->while_clause = until_clause;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_Command].value;
						break;
					}
					case 26: { /* subshell -> LPAREN compound_list RPAREN */
						da_pop(stack); //)
						ListP *subshell = da_pop(stack)->token.list;
						da_pop(stack); //(
						reduced_entry->token.type = Subshell;
						reduced_entry->token.list = subshell;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Subshell].value;
						break;
					}
					case 27: { /* compound_list -> linebreak term */
						ListP *term = da_pop(stack)->token.list;
						da_pop(stack); //linebreak
						reduced_entry->token.type = Compound_List;
						reduced_entry->token.list = term;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_List].value;
						break;
					}
					case 28: { /* compound_list -> linebreak term separator */
						TokenType sep = da_pop(stack)->token.separator_op;
						ListP *list = da_pop(stack)->token.list;
						da_pop(stack); //linebreak
						list->separator = sep;
						reduced_entry->token.type = Compound_List;
						reduced_entry->token.list = list;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Compound_List].value;
						break;
					}
					case 29: { /* term -> term separator and_or */
						AndOrP *and_or = da_pop(stack)->token.and_or;
						TokenType sep = da_pop(stack)->token.separator_op;
						ListP *term = da_pop(stack)->token.list;
						term->separator = sep;
						listAddBack(&term, listNew(and_or, END));
						reduced_entry->token.type = Term;
						reduced_entry->token.list = term;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Term].value;
						break;
					}
					case 30: { /* term -> and_or */
						AndOrP *and_or = da_pop(stack)->token.and_or;
						reduced_entry->token.type = Term;
						reduced_entry->token.list = listNew(and_or, END);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Term].value;
						break;
					}
					case 31: { /* for_clause -> FOR name do_group */
						ListP *do_group = da_pop(stack)->token.list;
						char *name = da_pop(stack)->token.raw_value;
						da_pop(stack); //FOR
						reduced_entry->token.type = For_Clause;
						reduced_entry->token.for_clause = gc_unique(ForClauseP, GC_SUBSHELL);
						reduced_entry->token.for_clause->iterator = name;
						reduced_entry->token.for_clause->body = do_group;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][For_Clause].value;
						break;
					}
					case 32: { /* for_clause -> FOR name sequential_sep do_group */
						ListP *do_group = da_pop(stack)->token.list;
						da_pop(stack); //sequential_sep
						char *name = da_pop(stack)->token.raw_value;
						da_pop(stack); //FOR
						reduced_entry->token.type = For_Clause;
						reduced_entry->token.for_clause = gc_unique(ForClauseP, GC_SUBSHELL);
						reduced_entry->token.for_clause->body = do_group;
						reduced_entry->token.for_clause->iterator = name;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][For_Clause].value;
						break;
					}
					case 33: { /* for_clause -> FOR name linebreak in sequential_sep do_group */
						ListP *do_group = da_pop(stack)->token.list;
						da_pop(stack); //sequential_sep
						da_pop(stack); //in
						da_pop(stack); //linebreak
						char *name = da_pop(stack)->token.raw_value;
						da_pop(stack); //FOR
						reduced_entry->token.type = For_Clause;
						reduced_entry->token.for_clause = gc_unique(ForClauseP, GC_SUBSHELL);
						reduced_entry->token.for_clause->iterator = name;
						reduced_entry->token.for_clause->body = do_group;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][For_Clause].value;
						break;
					}
					case 34: { /* for_clause -> FOR name linebreak in wordlist sequential_sep do_group */
						ListP *do_group = da_pop(stack)->token.list;
						da_pop(stack); //sequential_sep
						StringListL *word_list = da_pop(stack)->token.word_list;
						da_pop(stack); //in
						da_pop(stack); //linebreak
						char *iterator = da_pop(stack)->token.raw_value;
						da_pop(stack); //FOR
						reduced_entry->token.type = For_Clause;
						reduced_entry->token.for_clause = gc_unique(ForClauseP, GC_SUBSHELL);
						reduced_entry->token.for_clause->iterator = iterator;
						reduced_entry->token.for_clause->word_list = word_list;
						reduced_entry->token.for_clause->body = do_group;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][For_Clause].value;
						break;
					}
					case 35: { /* name -> NAME */
						char *name = da_pop(stack)->token.raw_value;
						reduced_entry->token.type = Name;
						reduced_entry->token.raw_value = name;
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
						char *word = da_pop(stack)->token.raw_value;
						StringListL *word_list = da_pop(stack)->token.word_list;
						da_push(word_list, word);
						reduced_entry->token.type = Wordlist;
						reduced_entry->token.word_list = word_list;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Wordlist].value;
						break;
					}
					case 38: { /* wordlist -> WORD */
						char *word = da_pop(stack)->token.raw_value;
						reduced_entry->token.type = Wordlist;
						da_create(word_list, StringListL, sizeof(char *), GC_SUBSHELL);
						da_push(word_list, word);
						reduced_entry->token.word_list = word_list;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Wordlist].value;
						break;
					}
					case 39: { /* case_clause -> CASE WORD linebreak in linebreak case_list ESAC */
						da_pop(stack); //ESAC
						CaseClauseP *case_list = da_pop(stack)->token.case_clause;
						da_pop(stack); //linebreak
						da_pop(stack); //in
						da_pop(stack); //linebreak
						char *expression = da_pop(stack)->token.raw_value;
						da_pop(stack); //CASE
						reduced_entry->token.type = Case_Clause;
						reduced_entry->token.case_clause = case_list;
						reduced_entry->token.case_clause->expression = expression;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Clause].value;
						break;
					}
					case 40: { /* case_clause -> CASE WORD linebreak in linebreak case_list_ns ESAC */
						da_pop(stack); //ESAC
						CaseClauseP *case_list = da_pop(stack)->token.case_clause;
						da_pop(stack); //linebreak
						da_pop(stack); //in
						da_pop(stack); //linebreak
						char *expression = da_pop(stack)->token.raw_value;
						da_pop(stack); //CASE
						reduced_entry->token.type = Case_Clause;
						reduced_entry->token.case_clause = case_list;
						reduced_entry->token.case_clause->expression = expression;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Clause].value;
						break;
					}
					case 41: { /* case_clause -> CASE WORD linebreak in linebreak ESAC */
						da_pop(stack); //ESAC
						da_pop(stack); //linebreak
						da_pop(stack); //in
						da_pop(stack); //linebreak
						char *expression = da_pop(stack)->token.raw_value;
						da_pop(stack); //CASE
						reduced_entry->token.type = Case_Clause;
						reduced_entry->token.case_clause = caseClauseNew(NULL, NULL);
						reduced_entry->token.case_clause->expression = expression;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Clause].value;
						break;
					}
					case 42: { /* case_list_ns -> case_list case_item_ns */
						CaseClauseP *case_list = da_pop(stack)->token.case_clause;
						CaseClauseP *case_item_ns = da_pop(stack)->token.case_clause;
						caseClauseMerge(case_list, case_item_ns);
						reduced_entry->token.type = Case_List_Ns;
						reduced_entry->token.case_clause = case_list;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_List_Ns].value;
						break;
					}
					case 43: { /* case_list_ns -> case_item_ns */
						CaseClauseP *case_item_ns = da_pop(stack)->token.case_clause;
						reduced_entry->token.type = Case_List_Ns;
						reduced_entry->token.case_clause = case_item_ns;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_List_Ns].value;
						break;
					}
					case 44: { /* case_list -> case_list case_item */
						CaseClauseP *case_list = da_pop(stack)->token.case_clause;
						CaseClauseP *case_item = da_pop(stack)->token.case_clause;
						caseClauseMerge(case_list, case_item);
						reduced_entry->token.type = Case_List;
						reduced_entry->token.case_clause = case_list;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_List].value;
						break;
					}
					case 45: { /* case_list -> case_item */
						CaseClauseP *case_item = da_pop(stack)->token.case_clause;
						reduced_entry->token.type = Case_List;
						reduced_entry->token.case_clause = case_item;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_List].value;
						break;
					}
					case 46: { /* case_item_ns -> pattern RPAREN linebreak */
						da_pop(stack); //linebreak
						da_pop(stack); //RPAREN
						StringListL *pattern = da_pop(stack)->token.word_list;
						reduced_entry->token.type = Case_Item_Ns;
						reduced_entry->token.case_clause = caseClauseNew(pattern, NULL);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item_Ns].value;
						break;
					}
					case 47: { /* case_item_ns -> pattern RPAREN compound_list */
						ListP *body = da_pop(stack)->token.list;
						da_pop(stack); //RPAREN
						StringListL *pattern = da_pop(stack)->token.word_list;
						reduced_entry->token.type = Case_Item_Ns;
						reduced_entry->token.case_clause = caseClauseNew(pattern, body);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item_Ns].value;
						break;
					}
					case 48: { /* case_item_ns -> LPAREN pattern RPAREN linebreak */
						da_pop(stack); //linebreak
						da_pop(stack); //RPAREN
						StringListL *pattern = da_pop(stack)->token.word_list;
						da_pop(stack); //LPAREN
						reduced_entry->token.type = Case_Item_Ns;
						reduced_entry->token.case_clause = caseClauseNew(pattern, NULL);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item_Ns].value;
						break;
					}
					case 49: { /* case_item_ns -> LPAREN pattern RPAREN linebreak */
						da_pop(stack); //linebreak
						da_pop(stack); //RPAREN
						StringListL *pattern = da_pop(stack)->token.word_list;
						da_pop(stack); //LPAREN
						reduced_entry->token.type = Case_Item_Ns;
						reduced_entry->token.case_clause = caseClauseNew(pattern, NULL);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item_Ns].value;
						break;
					}
					case 50: { /* case_item_ns -> LPAREN pattern RPAREN compound_list */
						ListP *body = da_pop(stack)->token.list;
						da_pop(stack); //RPAREN
						StringListL *pattern = da_pop(stack)->token.word_list;
						da_pop(stack); //LPAREN
						reduced_entry->token.type = Case_Item_Ns;
						reduced_entry->token.case_clause = caseClauseNew(pattern, body);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item_Ns].value;
						break;
					}
					case 51: { /* case_item -> pattern RPAREN linebreak DSEMI linebreak */
						da_pop(stack); //linebreak
						da_pop(stack); //DSEMI
						da_pop(stack); //linebreak
						da_pop(stack); //RPAREN
						StringListL *pattern = da_pop(stack)->token.word_list;
						reduced_entry->token.type = Case_Item;
						reduced_entry->token.case_clause = caseClauseNew(pattern, NULL);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item].value;
						break;
					}
					case 52: { /* case_item -> pattern RPAREN compound_list DSEMI linebreak */
						da_pop(stack); //linebreak
						da_pop(stack); //DSEMI
						ListP *body = da_pop(stack)->token.list;
						da_pop(stack); //PAREN
						StringListL *pattern = da_pop(stack)->token.word_list;
						reduced_entry->token.type = Case_Item;
						reduced_entry->token.case_clause = caseClauseNew(pattern, body);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item].value;
						break;
					}
					case 53: { /* case_item -> LPAREN pattern RPAREN linebreak DSEMI linebreak */
						da_pop(stack); //linebreak
						da_pop(stack); //DSEMI
						da_pop(stack); //linebreak
						da_pop(stack); //RPAREN
						StringListL *pattern = da_pop(stack)->token.word_list;
						da_pop(stack); //LPAREN
						reduced_entry->token.type = Case_Item;
						reduced_entry->token.case_clause = caseClauseNew(pattern, NULL);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item].value;
						break;
					}
					case 54: { /* case_item -> LPAREN pattern RPAREN compound_list DSEMI linebreak */
						da_pop(stack); //linebreak
						da_pop(stack); //DSEMI
						ListP *body = da_pop(stack)->token.list;
						da_pop(stack); //PAREN
						StringListL *pattern = da_pop(stack)->token.word_list;
						da_pop(stack);
						reduced_entry->token.type = Case_Item;
						reduced_entry->token.case_clause = caseClauseNew(pattern, body);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Case_Item].value;
						break;
					}
					case 55: { /* pattern -> WORD */
						char *word = da_pop(stack)->token.raw_value;
						reduced_entry->token.type = Pattern;
						da_create(word_list, StringListL, sizeof(char *), GC_SUBSHELL);
						da_push(word_list, word);
						reduced_entry->token.word_list = word_list;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pattern].value;
						break;
					}
					case 56: { /* pattern -> pattern PIPE WORD */
						char *word = da_pop(stack)->token.raw_value;
						da_pop(stack); //PIPE
						StringListL *pattern = da_pop(stack)->token.word_list;
						da_push(pattern, word);
						reduced_entry->token.type = Pattern;
						reduced_entry->token.word_list = pattern;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Pattern].value;
						break;
					}
					case 57: { /* if_clause -> IF compound_list THEN compound_list else_part FI */
						da_pop(stack); //FI
						IFClauseP *else_part = da_pop(stack)->token.if_clause;
						ListP *first_body = da_pop(stack)->token.list;
						da_pop(stack); //THEN
						ListP *first_condition = da_pop(stack)->token.list;
						da_pop(stack); //IF
						da_push_front(else_part->bodies, first_body);
						da_push_front(else_part->conditions, first_condition);
						reduced_entry->token.type = If_Clause;
						reduced_entry->token.if_clause = else_part;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][If_Clause].value;
						break;
					}
					case 58: { /* if_clause -> IF compound_list THEN compound_list FI */
						da_pop(stack); //FI
						ListP *body = da_pop(stack)->token.list;
						da_pop(stack); //THEN
						ListP *condition = da_pop(stack)->token.list;
						print_list(condition);
						da_pop(stack); //IF
						reduced_entry->token.type = If_Clause;
						reduced_entry->token.if_clause = ifClauseNew(condition, body);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][If_Clause].value;
						break;
					}
					case 59: { /* else_part -> ELIF compound_list THEN compound_list */
						ListP *body = da_pop(stack)->token.list;
						da_pop(stack); //THEN
						ListP *condition = da_pop(stack)->token.list;
						da_pop(stack); //ELIF
						reduced_entry->token.type = Else_Part;
						reduced_entry->token.if_clause = ifClauseNew(condition, body);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Else_Part].value;
						break;
					}
					case 60: { /* else_part -> ELIF compound_list THEN compound_list else_part */
						IFClauseP *else_part = da_pop(stack)->token.if_clause;
						ListP *anterior_body = da_pop(stack)->token.list;
						da_pop(stack); //THEN
						ListP *anterior_condition = da_pop(stack)->token.list;
						da_pop(stack); //ELIF
						da_push_front(else_part->conditions, anterior_condition);
						da_push_front(else_part->bodies, anterior_body);
						reduced_entry->token.type = Else_Part;
						reduced_entry->token.if_clause = else_part;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Else_Part].value;
						break;
					}
					case 61: { /* else_part -> ELSE compound_list */
						ListP *final_body = da_pop(stack)->token.list;
						da_pop(stack); //ELSE
						reduced_entry->token.type = Else_Part;
						reduced_entry->token.if_clause = ifClauseNew(NULL, final_body);
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Else_Part].value;
						break;
					}
					case 62: { /* while_clause -> WHILE compound_list do_group */
						ListP *do_group = da_pop(stack)->token.list;
						ListP *compound_list = da_pop(stack)->token.list;
						da_pop(stack); //WHILE
						reduced_entry->token.type = While_Clause;
						reduced_entry->token.while_clause = gc_unique(WhileClauseP, GC_SUBSHELL);
						reduced_entry->token.while_clause->body = do_group;
						reduced_entry->token.while_clause->condition = compound_list;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][While_Clause].value;
						break;
					}
					case 63: { /* until_clause -> UNTIL compound_list do_group */
						ListP *do_group = da_pop(stack)->token.list;
						ListP *compound_list = da_pop(stack)->token.list;
						da_pop(stack); //UNTIL
						reduced_entry->token.type = Until_Clause;
						reduced_entry->token.while_clause = gc_unique(WhileClauseP, GC_SUBSHELL);
						reduced_entry->token.while_clause->body = do_group;
						reduced_entry->token.while_clause->condition = compound_list;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Until_Clause].value;
						break;
					}
					case 64: { /* function_definition -> fname LPAREN RPAREN linebreak function_body */
						CommandP *function_body = da_pop(stack)->token.command;
						da_pop(stack); //linebreak
						da_pop(stack); //RPAREN
						da_pop(stack); //LPAREN
						char *fname = da_pop(stack)->token.raw_value;
						reduced_entry->token.type = Function_Definition;
						reduced_entry->token.command = gc_unique(CommandP, GC_SUBSHELL);
						reduced_entry->token.command->type = Function_Definition;
						reduced_entry->token.command->function_definition = gc_unique(FunctionP, GC_SUBSHELL);
						reduced_entry->token.command->function_definition->function_body = function_body;
						reduced_entry->token.command->function_definition->function_name = fname;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Function_Definition].value;
						break;
					}
					case 65: { /* function_body -> compound_command */
						CommandP *command = da_pop(stack)->token.command;
						reduced_entry->token.type = Function_Body;
						reduced_entry->token.command = command;
						reduced_entry->token.command->type = Brace_Group;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Function_Body].value;
						break;
					}
					case 66: { /* function_body -> compound_command redirect_list */
						RedirectionL *redir_list = da_pop(stack)->token.redir_list;
						CommandP *command = da_pop(stack)->token.command;
						command->redir_list = redir_list;
						reduced_entry->token.type = Function_Body;
						reduced_entry->token.command = command;
						reduced_entry->token.command->type = Brace_Group;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Function_Body].value;
						break;
					}
					case 67: { /* fname -> NAME */
						char *name = da_pop(stack)->token.raw_value;
						reduced_entry->token.type = Fname;
						reduced_entry->token.raw_value = name;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Fname].value;
						break;
					}
					case 68: { /* brace_group -> LBRACE compound_list RBRACE */
						da_pop(stack); //LBRACE
						ListP *compound_list = da_pop(stack)->token.list;
						da_pop(stack); //RBRACE
						reduced_entry->token.type = Brace_Group;
						reduced_entry->token.list = compound_list;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Brace_Group].value;
						break;
					}
					case 69: { /* do_group -> DO compound_list DONE */
						da_pop(stack); //DONE
						ListP *compound_list = da_pop(stack)->token.list;
						da_pop(stack); //DO
						reduced_entry->token.type = Do_Group;
						reduced_entry->token.list = compound_list;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Do_Group].value;
						break;
					}
					case 70: { /* simple_command -> cmd_prefix cmd_word cmd_suffix */
						SimpleCommandP *suffix = da_pop(stack)->token.simple_command;
						char *word = da_pop(stack)->token.raw_value;
						SimpleCommandP *prefix = da_pop(stack)->token.simple_command;
						da_push(prefix->word_list, word);
						for (size_t i = 0; i < suffix->word_list->size; i++)
							da_push(prefix->word_list, suffix->word_list->data[i]);
						for (size_t i = 0; i < suffix->redir_list->size; i++)
							da_push(prefix->redir_list, suffix->redir_list->data[i]);
						reduced_entry->token.type = Simple_Command;
						reduced_entry->token.simple_command = prefix;
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
						SimpleCommandP *command = da_pop(stack)->token.simple_command;
						reduced_entry->token.type = Simple_Command;
						reduced_entry->token.simple_command = command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Simple_Command].value;
						break;
					}
					case 73: { /* simple_command -> cmd_name cmd_suffix */
						SimpleCommandP *command = da_pop(stack)->token.simple_command;
						char *word = da_pop(stack)->token.raw_value;
						da_push_front(command->word_list, word);
						reduced_entry->token.type = Simple_Command;
						reduced_entry->token.simple_command = command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Simple_Command].value;
						break;
					}
					case 74: { /* simple_command -> cmd_name */
						char *word = da_pop(stack)->token.raw_value;
						SimpleCommandP *command = simpleCommandNew();
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
						SimpleCommandP *command = simpleCommandNew();
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
						char *word = da_pop(stack)->token.raw_value;
						SimpleCommandP *command = simpleCommandNew();
						da_push(command->assign_list, word);
						reduced_entry->token.type = Cmd_Prefix;
						reduced_entry->token.simple_command = command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Prefix].value;
						break;
					}
					case 80: { /* cmd_prefix -> cmd_prefix ASSIGNMENT_WORD */
						char *word = da_pop(stack)->token.raw_value;
						SimpleCommandP *command = da_pop(stack)->token.simple_command;
						da_push(command->assign_list, word);	
						reduced_entry->token.type = Cmd_Prefix;
						reduced_entry->token.simple_command = command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Prefix].value;
						break;
					}
					case 81: { /* cmd_suffix -> io_redirect */
						RedirectionP *redir = da_pop(stack)->token.redir;
						SimpleCommandP *command = simpleCommandNew();
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
						SimpleCommandP *command = simpleCommandNew();
						da_push(command->word_list, word);
						reduced_entry->token.type = Cmd_Suffix;
						reduced_entry->token.simple_command = command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Suffix].value;
						break;
					}
					case 84: { /* cmd_suffix -> cmd_suffix WORD */
						char *word = da_pop(stack)->token.raw_value;
						SimpleCommandP *command = da_pop(stack)->token.simple_command;
						da_push(command->word_list, word);
						reduced_entry->token.type = Cmd_Suffix;
						reduced_entry->token.simple_command = command;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Cmd_Suffix].value;
						break;
					}
					case 85: { /* redirect_list -> io_redirect */
						RedirectionP *io_redirect = da_pop(stack)->token.redir;
						reduced_entry->token.type = Redirect_List;
						da_create(redir_list, RedirectionL, sizeof(RedirectionP *), GC_SUBSHELL);
						da_push(redir_list, io_redirect);
						reduced_entry->token.redir_list = redir_list;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Redirect_List].value;
						break;
					}
					case 86: { /* redirect_list -> redirect_list io_redirect */
						RedirectionP *io_redirect = da_pop(stack)->token.redir;
						RedirectionL *redir_list = da_pop(stack)->token.redir_list;
						da_push(redir_list, io_redirect);
						reduced_entry->token.type = Redirect_List;
						reduced_entry->token.redir_list = redir_list;
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
						RedirectionP *io_file = da_pop(stack)->token.redir;
						char *io_number = da_pop(stack)->token.raw_value;
						io_file->io_number = io_number;
						reduced_entry->token.type = Io_Redirect;
						reduced_entry->token.redir = io_file;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_Redirect].value;
						break;
					}
					case 90: { /* io_redirect -> IO_NUMBER io_here */
						RedirectionP *io_here = da_pop(stack)->token.redir;
						char *io_number = da_pop(stack)->token.raw_value;
						io_here->io_number = io_number;
						reduced_entry->token.type = Io_Redirect;
						reduced_entry->token.redir = io_here;
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
						reduced_entry->token.redir = gc_unique(RedirectionP, GC_SUBSHELL);
						reduced_entry->token.redir->filename = filename.raw_value;
						reduced_entry->token.redir->type = type.type;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_File].value;
						break;
					}
					case 99: /* io_here -> DLESS here_end *//*      TODO: start here_doc */
					case 100: { /* io_here -> DLESSDASH here_end */ //  TODO: start here_doc no expansions */
						Tokenn here_end = da_pop(stack)->token;
						TokenType type = da_pop(stack)->token.type;

						char *heredoc_filename = NULL;
						switch (type) {
							case DLESS: //<<
								{
									// start_here_doc expansion on
									heredoc_filename = here_doc(here_end.raw_value, HD_EXPAND, shell_vars);
									break;
								}
							case DLESSDASH:
								{
									// start_here_doc expansions off
									heredoc_filename = here_doc(here_end.raw_value, HD_NO_EXPAND, shell_vars);
									break;
								}
							default: {};
						}
						if (!heredoc_filename){
							return NULL;
						}
						//start here doc probleme reutrn NULL
						reduced_entry->token.type = Io_Redirect;
						reduced_entry->token.redir = gc_unique(RedirectionP, GC_SUBSHELL);
						reduced_entry->token.redir->filename = heredoc_filename;
						reduced_entry->token.redir->type = LESS; // <
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_Redirect].value;
						break;
					}
					case 98: { /* filename -> WORD */
						Tokenn word = da_pop(stack)->token;
						reduced_entry->token.type = Filename;
						reduced_entry->token.raw_value = word.raw_value;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Filename].value;
						break;
					}
					case 101: { /* here_end -> WORD */
						//TODO: Directly apply quote removal cuz of no expansions
						Tokenn word = da_pop(stack)->token;
						reduced_entry->token.type = Here_End;
						reduced_entry->token.raw_value = word.raw_value;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Here_End].value;
						break;
					}
					case 102: { /* newline_list -> NEWLINE */
						da_pop(stack);
						reduced_entry->token.type = Newline_List;
						reduced_entry->token.separator_op = NEWLINE;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Newline_List].value;
						break;
					}
					case 103: { /* newline_list -> newline_list NEWLINE */
						da_pop(stack);
						da_pop(stack);
						reduced_entry->token.type = Newline_List;
						reduced_entry->token.separator_op = NEWLINE;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Newline_List].value;
						break;
					}
					case 104: { /* linebreak -> newline_list */
						da_pop(stack);
						reduced_entry->token.type = Linebreak;
						reduced_entry->token.separator_op = NEWLINE;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Linebreak].value;
						break;
					}
					case 105: { /* linebreak -> '' */
						// da_pop(stack); <-- Epsilon can't be popped :)
						reduced_entry->token.type = Linebreak;
						reduced_entry->token.separator_op = END;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Linebreak].value;
						break;
					}
					case 106: { /* separator_op -> AMPER */
						da_pop(stack); //AMPER
						reduced_entry->token.type = Separator_Op;
						reduced_entry->token.separator_op = AMPER;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Separator_Op].value;
						break;
					}
					case 107: { /* separator_op -> SEMI */
						da_pop(stack); //SEMI
						reduced_entry->token.type = Separator_Op;
						reduced_entry->token.separator_op = SEMI;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Separator_Op].value;
						break;
					}
					case 108: { /* separator -> separator_op linebreak */
						da_pop(stack); //linebreak
						TokenType sep = da_pop(stack)->token.separator_op;
						reduced_entry->token.type = Separatore;
						reduced_entry->token.separator_op = sep;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Separatore].value;
						break;
					}
					case 109: { /* separator -> newline_list */
						da_pop(stack); //newline_list
						reduced_entry->token.type = Separatore;
						reduced_entry->token.separator_op = NEWLINE;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Separatore].value;
						break;
					}
					case 110: { /* sequential_sep -> SEMI linebreak */
						da_pop(stack); //linebreak
						da_pop(stack); //SEMI
						reduced_entry->token.type = Sequential_Sep;
						reduced_entry->token.separator_op = SEMI;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Sequential_Sep].value;
						break;
					}
					case 111: { /* sequential_sep -> newline_list */
						da_pop(stack); //newline_list
						reduced_entry->token.type = Sequential_Sep;
						reduced_entry->token.separator_op = NEWLINE;
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
				if (ft_strcmp(token.raw_value, ""))
					pretty_error(token.raw_value);
				return NULL;
				break;
			}
		}
	}
	return NULL;
}


void parse_input(char *in, char *filename, Vars *shell_vars) {
	lex_interface(LEX_SET, in, filename, NULL);
	// dprintf(2, "input:\n%s", in);
	// for (size_t i = 0; i < 100; i++) {
	// 	char *value = lex_interface(LEX_GET, NULL, error);	
	// 	TokenType type = identify_token(value, 0);
	// 	dprintf(2, "|%s| -> %s\n", value, tokenTypeStr(type));
	// }
	parse(shell_vars);
}
