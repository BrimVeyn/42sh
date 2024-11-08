/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_parser_debug.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 14:43:08 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/08 10:17:50 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "utils.h"
#include "colors.h"

#include <stdlib.h>
#include <stdio.h>

void pretty_error(char *raw_token) {
	Lex *lexer = lex_interface(LEX_OWN, NULL, NULL);
	char buffer[MAX_WORD_LEN] = {0};
	int message_len = ft_sprintf(buffer, "%s:%ld:%ld: "C_BOLD C_BRIGHT_RED"error"C_RESET": unexpected token near: "C_BOLD C_WHITE"\'%s\'"C_RESET"\n", lexer->filename, lexer->line, lexer->column, raw_token);
	int lineno_len = ft_sprintf(buffer, "  %ld", lexer->line) - message_len;
	ft_sprintf(buffer, "  |  ");
	char *line = get_line_x(lexer->raw_input, lexer->line);
	int tab_count = 0;
	for (size_t i = 0; line[i]; i++) {
		if (line[i] == '\t') {
			tab_count += 1;
		}
	}
	ft_sprintf(buffer, "%s\n", line);
	for(int i = 0; i < lineno_len; i++) {
		ft_sprintf(buffer, " ");
	}
	ft_sprintf(buffer, "  |  ");
	for (size_t i = 0; i < lexer->column; i++) {
		if (tab_count) {
			ft_sprintf(buffer, "\t");
			tab_count -= 1;
		} else {
			ft_sprintf(buffer, " ");
		}
	}
	ft_sprintf(buffer, C_BOLD C_BRIGHT_RED"^"C_RESET"\n");
	ft_dprintf(2, buffer);
	FREE_POINTERS(line);
}

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

void print_word_list(const char *title, const StringListL *word_list) {
	dprintf(2, C_DARK_CYAN"  %s: "C_RESET C_SEA_GREEN"{"C_RESET, title);
	if (!word_list) {
		dprintf(2, "(null)");
		goto close_print;
	}
	for (size_t i = 0; i < word_list->size; i++) {
		if (!word_list->data[i])
			continue;
		dprintf(2, "%s", word_list->data[i]);
		if (i < word_list->size - 1) {
			dprintf(2, ", ");
		}
	}
close_print:
	dprintf(2, C_SEA_GREEN"}"C_RESET"\n");
}

void print_simple_command(const SimpleCommandP *self) {
	dprintf(2, "  Bin: %s\n", self->bin);
	print_word_list("Vars", self->assign_list);
	print_word_list("Words", self->word_list);
	print_redir_list(self->redir_list);
}

void print_subshell(const ListP *subshell) {
	dprintf(2, C_CERULEAN"----------Subshell---------"C_RESET"\n");
	print_list(subshell);
	dprintf(2, C_CERULEAN"--------SubShellEnd----------"C_RESET"\n");
}

void print_brace_group(const ListP *subshell) {
	dprintf(2, C_CERULEAN"----------BraceGroup---------"C_RESET"\n");
	print_list(subshell);
	dprintf(2, C_CERULEAN"--------BraceGroupEnd----------"C_RESET"\n");
}

void print_for_clause(const ForClauseP *for_clause) {
	dprintf(2, C_CERULEAN"----------ForClause---------"C_RESET"\n");
	dprintf(2, C_DARK_CYAN"  Iterator:"C_RESET" %s\n", for_clause->iterator);
	if (for_clause->word_list)
		print_word_list("Enum", for_clause->word_list);
	print_list(for_clause->body);
	dprintf(2, C_CERULEAN"--------ForClauseEnd----------"C_RESET"\n");
}

void print_case_clause(const CaseClauseP *case_clause) {
	dprintf(2, C_CERULEAN"----------CaseClause---------"C_RESET"\n");
	dprintf(2, C_DARK_CYAN"  Expression:"C_RESET" %s\n", case_clause->expression);
	for(size_t i = 0; i < case_clause->patterns->size; i++) {
		print_word_list("Case", case_clause->patterns->data[i]);
	}
	for (size_t i = 0; i < case_clause->bodies->size; i++) {
		print_list(case_clause->bodies->data[i]);
	}
	dprintf(2, C_CERULEAN"--------CaseClauseEnd----------"C_RESET"\n");

}


void print_command(const CommandP *command) {
	switch (command->type) {
		case Simple_Command: print_simple_command(command->simple_command); break;
		case If_Clause: print_if_clause(command->if_clause); break;
		case While_Clause: print_while_clause(command->while_clause); break;
		case Until_Clause: print_while_clause(command->while_clause); break;
		case Subshell: print_subshell(command->subshell); break;
		case For_Clause: print_for_clause(command->for_clause); break;
		case Case_Clause: print_case_clause(command->case_clause); break;
		case Function_Definition: print_function(command->function_definition); break;
		case Brace_Group: print_brace_group(command->brace_group); break;
		default: break;
	}
	if (command->redir_list) {
		print_redir_list(command->redir_list);
	}
}

void print_function(const FunctionP *function) {
	dprintf(2, C_CERULEAN"----------FunctionDef---------"C_RESET"\n");
	dprintf(2, C_DARK_PINK"  Fname:"C_RESET" %s\n", function->function_name);
	print_command(function->function_body);
	dprintf(2, C_CERULEAN"--------FunctionDefEnd----------"C_RESET"\n");
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
	if (!head) {
		dprintf(2, "  Body: (null)\n");
	}
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
				case Function_Definition:
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
				case Case_Item:
				case Case_Item_Ns:
				case Case_List:
				case Case_List_Ns: {
					print_case_clause(entry->token.case_clause);
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
