/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:19:12 by nbardavi          #+#    #+#             */
/*   Updated: 2024/11/06 10:14:16 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "ft_regex.h"
#include "utils.h"
#include "libft.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

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

typedef struct IFClauseP IFClauseP;
typedef struct ForClauseP ForClauseP;
typedef struct WhileClauseP WhileClauseP;
typedef struct PipeLineP PipeLineP;
typedef struct AndOrP AndOrP;
typedef struct ListP ListP;

typedef struct {
	TokenType type;
	RedirectionL *redir_list;
	union {
		SimpleCommandP *simple_command;
		ListP *subshell;
		ForClauseP *for_clause;
		IFClauseP *if_clause;
		WhileClauseP *while_clause;
		// BraceGroupP *brace_group;
		// CaseClause *case_clause;
	};
} CommandP;

typedef struct {
	ListP *list;
	TokenType separator;
} CompleteCommandP;

struct PipeLineP {
	bool banged;
	CommandP *command;
	PipeLineP *next;
};

struct AndOrP {
	PipeLineP *pipeline;
	TokenType separator;
	AndOrP *next;
};

struct ListP {
	AndOrP *and_or;
	TokenType separator;
	ListP *next;
};

typedef struct {
	ListP **data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int	gc_level;
} ListPVect;

struct IFClauseP {
	ListPVect *conditions;
	ListPVect *bodies;
};

struct WhileClauseP {
	ListP *condition;
	ListP *body;
};

struct ForClauseP {
	char *iterator;
	ListP *body;
	StringListL *word_list;
};

typedef struct {
	TokenType type;
	union {
		StringListL *word_list;
		WhileClauseP *while_clause;
		ForClauseP *for_clause;
		IFClauseP *if_clause;
		TokenType separator_op;
		ListP *list;
		AndOrP *and_or;
		PipeLineP *pipeline;
		CommandP *command;
		char *raw_value;
		RedirectionP *redir;
		FilenameP *filename;
		RedirectionL *redir_list;
		SimpleCommandP *simple_command;
		CompleteCommandP *complete_command;
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
void	pipelineAddBack(PipeLineP **lst, PipeLineP *new_value) {
	PipeLineP	*temp;

	if (*lst == NULL)
		*lst = new_value;
	else {
		temp = *lst;
		while (temp->next)
			temp = temp->next;
		temp->next = new_value;
	}
}

PipeLineP *pipelineNew(CommandP *command) {
	PipeLineP *self = gc_unique(PipeLineP, GC_SUBSHELL);
	self->command = command;
	return self;
}

void	andOrAddBack(AndOrP **lst, AndOrP *new_value) {
	AndOrP	*temp;

	if (*lst == NULL)
		*lst = new_value;
	else {
		temp = *lst;
		while (temp->next)
			temp = temp->next;
		temp->next = new_value;
	}
}

AndOrP *andOrNew(PipeLineP *pipeline, TokenType separator) {
	AndOrP *self = gc_unique(AndOrP, GC_SUBSHELL);
	self->pipeline = pipeline;
	self->separator = separator;
	return self;
}

void	listAddBack(ListP **lst, ListP *new_value) {
	ListP	*temp;

	if (*lst == NULL)
		*lst = new_value;
	else {
		temp = *lst;
		while (temp->next)
			temp = temp->next;
		temp->next = new_value;
	}
}

ListP *listNew(AndOrP *and_or, TokenType separator) {
	ListP *self = gc_unique(AndOrP, GC_SUBSHELL);
	self->and_or = and_or;
	self->separator = separator;
	return self;
}

IFClauseP *IFClauseNew(ListP *condition, ListP *body) {
	IFClauseP *self = gc_unique(IFClauseP, GC_SUBSHELL);
	da_create(condition_list, ListPVect, sizeof(ListP *), GC_SUBSHELL);
	if (condition)
		da_push(condition_list, condition);
	da_create(body_list, ListPVect, sizeof(ListP *), GC_SUBSHELL);
	if (body)
		da_push(body_list, body);
	self->conditions = condition_list;
	self->bodies = body_list;
	return self;
}

void print_if_clause(const IFClauseP *if_clause);
void print_while_clause(const WhileClauseP *while_clause);
void print_list(const ListP *list);

void print_redir_list(const RedirectionL *redir_list) {
	dprintf(2, C_DARK_CYAN"  Redirs: "C_RESET C_SEA_GREEN"{"C_RESET);
	for (size_t i = 0; i < redir_list->size; i++) {
		const RedirectionP *redir = redir_list->data[i];
		if (redir->io_number) {
			dprintf(2, C_MAGENTA"{"C_RESET"%s | %s | %s"C_MAGENTA"}"C_RESET, redir->io_number, tokenTypeStr(redir->type), redir->filename);
		} else {
			dprintf(2, C_MAGENTA"{"C_RESET"%s | %s"C_MAGENTA"}"C_RESET, tokenTypeStr(redir->type), redir->filename);
		}
		if (i < redir_list->size - 1) {
			dprintf(2, ", ");
		}
	}
	dprintf(2, C_SEA_GREEN"}"C_RESET"\n");
}

void print_word_list(const StringListL *word_list) {
	dprintf(2, C_DARK_CYAN"  Words: "C_RESET C_SEA_GREEN"{"C_RESET);
	for (size_t i = 0; i < word_list->size; i++) {
		dprintf(2, "%s", word_list->data[i]);
		if (i < word_list->size - 1) {
			dprintf(2, ", ");
		}
	}
	dprintf(2, C_SEA_GREEN"}"C_RESET"\n");
}

void print_simple_command(const SimpleCommandP *self) {
	print_word_list(self->word_list);
	print_redir_list(self->redir_list);
}

void print_subshell(const ListP *subshell) {
	dprintf(2, C_CERULEAN"----------Subshell---------"C_RESET"\n");
	print_list(subshell);
	dprintf(2, C_CERULEAN"--------SubShellEnd----------"C_RESET"\n");
}

void print_for_clause(const ForClauseP *for_clause) {
	dprintf(2, C_CERULEAN"----------ForClause---------"C_RESET"\n");
	print_list(for_clause->body);
	print_word_list(for_clause->word_list);
	dprintf(2, C_DARK_CYAN"  Iterator:"C_RESET" %s\n", for_clause->iterator);
	dprintf(2, C_CERULEAN"--------ForClauseEnd----------"C_RESET"\n");
}

void print_command(CommandP *command) {
	switch (command->type) {
		case Simple_Command: print_simple_command(command->simple_command); break;
		case If_Clause: print_if_clause(command->if_clause); break;
		case While_Clause: print_while_clause(command->while_clause); break;
		case Until_Clause: print_while_clause(command->while_clause); break;
		case Subshell: print_subshell(command->subshell); break;
		case For_Clause: print_for_clause(command->for_clause); break;
		default: break;
	}
	if (command->redir_list) {
		print_redir_list(command->redir_list);
	}
}

void print_pipeline(const PipeLineP *pipeline) {
	const PipeLineP *head = pipeline;
	while (head) {
		print_command(head->command);
		if (head->next)
			dprintf(2, C_ORCHID"  Sep: "C_MAGENTA"["C_LIGHT_ORANGE"%s"C_MAGENTA"]"C_RESET"\n", tokenTypeStr(PIPE));
		head = head->next;
	}
}

void print_andor(const AndOrP *and_or) {
	const AndOrP *head = and_or;
	while (head) {
		print_pipeline(head->pipeline);
		if (head->separator == AND_IF || head->separator == OR_IF) {
			dprintf(2, C_CORAL"  Sep: "C_MAGENTA"["C_LIGHT_ORANGE"%s"C_MAGENTA"]"C_RESET"\n", tokenTypeStr(head->separator));
		}
		head = head->next;
	}
}

void print_list(const ListP *list) {
	const ListP *head = list;
	while (head) {
		print_andor(head->and_or);
		if (head->separator == SEMI || head->separator == AMPER || head->separator == NEWLINE) {
			dprintf(2, C_CORAL"  Sep: "C_MAGENTA"["C_LIGHT_ORANGE"%s"C_MAGENTA"]"C_RESET"\n", tokenTypeStr(head->separator));
		}
		head = head->next;
	}
}

void print_complete_command(const CompleteCommandP *complete_command) {
	print_list(complete_command->list);
	if (complete_command->separator != END)
		dprintf(2, C_CORAL"  Sep: "C_MAGENTA"["C_LIGHT_ORANGE"%s"C_MAGENTA"]"C_RESET"\n", tokenTypeStr(complete_command->separator));
}

void print_if_clause(const IFClauseP *if_clause) {
	dprintf(2, C_CERULEAN"Conditions: \n");
	const ListPVect *conditions = if_clause->conditions;
	for (size_t i = 0; i < conditions->size; i++) {
		print_list(conditions->data[i]);
		dprintf(2, C_CERULEAN"---------------------------"C_RESET"\n");
	}
	dprintf(2, C_CERULEAN"Bodies: \n");
	const ListPVect *bodies = if_clause->bodies;
	for (size_t i = 0; i < bodies->size; i++) {
		print_list(bodies->data[i]);
		dprintf(2, C_CERULEAN"---------------------------"C_RESET"\n");
	}
}

void print_while_clause(const WhileClauseP *while_clause) {
	dprintf(2, C_EMERALD"---------------------------"C_RESET"\n");
	print_list(while_clause->condition);
	print_list(while_clause->body);
	dprintf(2, C_EMERALD"---------------------------"C_RESET"\n");
}

void print_token_stack(const TokenStack *stack) {
	for (size_t i = 0; i < stack->size; i++) {
		const StackEntry *entry = stack->data[i];

		if (entry->token.type <= END || entry->token.type == Filename) {
			dprintf(2, "["C_LIGHT_BLUE"%zu"C_RESET"] "C_MAGENTA"%s"C_LIGHT_YELLOW": "C_TURQUOISE"%s"C_RESET"\n", i, tokenTypeStr(entry->token.type), entry->token.raw_value);
		} else {
			dprintf(2, "["C_LIGHT_BLUE"%zu"C_RESET"] "C_LIGHT_YELLOW"%s"C_RESET"\n", i, tokenTypeStr(entry->token.type));
			switch (entry->token.type) {
				case Io_Redirect:
				case Io_File: {
					dprintf(2, C_MAGENTA"{"C_LIGHT_ORANGE"%s "C_RESET"|"C_LIGHT_ORANGE" %s"C_MAGENTA"}"C_RESET"\n", tokenTypeStr(entry->token.redir->type), entry->token.redir->filename);
					break;
				}
				case Cmd_Suffix:
				case Simple_Command:
				case Cmd_Prefix: {
					print_simple_command(entry->token.simple_command);
					break;
				}
				case Cmd_Name:
				case Cmd_Word: {
					dprintf(2, C_DARK_CYAN"  Value: "C_RESET C_MAGENTA"["C_LIGHT_ORANGE"%s"C_MAGENTA"]"C_RESET"\n", entry->token.raw_value);
					break;
				}
				case Redirect_List: {
					print_redir_list(entry->token.redir_list);
					break;
				}
				case Compound_Command:
				case Command: {
					print_command(entry->token.command);
					break;
				}
				case Pipe_Sequence:
				case Pipeline: {
					print_pipeline(entry->token.pipeline); 
					break; 
				}
				case And_Or: { 
					print_andor(entry->token.and_or);
					break;
				}
				case Term:
				case Compound_List:
				case Do_Group:
				case List: {
					print_list(entry->token.list);
					break;
				}
				case Else_Part:
				case If_Clause: {
					print_if_clause(entry->token.if_clause);
					break;
				}
				case Until_Clause:
				case While_Clause: {
					print_while_clause(entry->token.while_clause);
					break;
				}
				case Complete_Command: {
					print_complete_command(entry->token.complete_command);
					break;
				}
				default: break;
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
		[BANG] = "!", [PIPE] = "|",
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
		if (!ft_strcmp(map[i], raw_value) && 
			(table_row != 27 || i == PIPE 
			|| i == AND_IF || i == OR_IF ||
			i == SEMI || i == AMPER)) {
			return i;
		}
	}

	if (regex_match("^[0-9]+", (char *)raw_value).is_found) {
		char *peak = lex_interface(LEX_PEAK_CHAR, NULL);
		if (peak && (*peak == '<' || *peak == '>'))
			return IO_NUMBER;
	}

	if (regex_match("^[a-zA-Z][a-zA-Z0-9]*$", (char *)raw_value).is_found) {
		if (table_row == 30)
			return NAME;
	}

	return WORD;
}

extern TableEntry parsingTable[182][86];

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

	size_t action_no = 0;
	while (true && ++action_no < 200) {
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
						and_or->separator = separator;
						andOrAddBack(&and_or, andOrNew(pipeline, END));
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
						da_pop(stack);
						reduced_entry->token.type = Compound_Command;
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
						da_pop(stack);
						list->separator = sep;
						reduced_entry->token.type = Compound_List;
						reduced_entry->token.list = list;
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
						AndOrP *and_or = da_pop(stack)->token.and_or;
						reduced_entry->token.type = Term;
						reduced_entry->token.list = listNew(and_or, END);
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
						reduced_entry->token.if_clause = IFClauseNew(condition, body);
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
						reduced_entry->token.if_clause = IFClauseNew(condition, body);
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
						reduced_entry->token.if_clause = IFClauseNew(NULL, final_body);
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
					case 99: /* io_here -> DLESS here_end */ /*      TODO: start here_doc */
					case 100: { /* io_here -> DLESSDASH here_end */ //  TODO: start here_doc no expansions */
						Tokenn here_end = da_pop(stack)->token;
						Tokenn type = da_pop(stack)->token;
						reduced_entry->token.type = Io_File;
						reduced_entry->token.redir = gc_unique(RedirectionP, GC_SUBSHELL);
						reduced_entry->token.redir->filename = here_end.raw_value;
						reduced_entry->token.redir->type = type.type;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Io_Here].value;
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
						reduced_entry->token.type = Separator;
						reduced_entry->token.separator_op = sep;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Separator].value;
						break;
					}
					case 109: { /* separator -> newline_list */
						da_pop(stack); //newline_list
						reduced_entry->token.type = Separator;
						reduced_entry->token.separator_op = NEWLINE;
						state = da_peak_back(stack)->state;
						reduced_entry->state = parsingTable[state][Separator].value;
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
