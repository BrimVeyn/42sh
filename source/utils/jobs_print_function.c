/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jobs_print_function.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 16:23:39 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/29 23:36:10 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "utils.h"
#include "dynamic_arrays.h"

void job_print_list(ListP *list, StringStream *ss);
void job_print_function(FunctionP *func, StringStream *ss);

void job_print_stringlist(StringList *stringList, StringStream *ss) {
	for (size_t i = 0; i < stringList->size; i++) {
		ss_push_string(ss, stringList->data[i]);
		da_push(ss, ' ');
	}
}

void job_print_redirlist(RedirectionL *redirList, StringStream *ss) {
	for (size_t i = 0; i < redirList->size; i++) {
		if (redirList->data[i]->io_number) {
			ss_push_string(ss, redirList->data[i]->io_number);
			da_push(ss, ' ');
		}
		ss_push_string(ss, tokenTypeStr(redirList->data[i]->type));
		da_push(ss, ' ');
		ss_push_string(ss, redirList->data[i]->filename);
		da_push(ss, ' ');
	}
}

void job_print_listvect(ListPVect *vect, StringStream *ss) {
	for (size_t i = 0; i < vect->size; i++) {
		job_print_list(vect->data[i], ss);
	}
}

void job_print_stringlistvect(StringListVect *vect, StringStream *ss) {
	for (size_t i = 0; i < vect->size; i++) {
		job_print_stringlist(vect->data[i], ss);
	}
}

void job_print_if(IFClauseP *if_clause, StringStream *ss) {
	size_t i;
	for (i = 0; i < if_clause->bodies->size; i++) {
		if (i == 0)
			ss_push_string(ss, "if ");
		else if (i < if_clause->conditions->size)
			ss_push_string(ss, "\nelif ");
		else 
			ss_push_string(ss, "\nelse ");
		job_print_list(if_clause->conditions->data[i], ss);
		ss_push_string(ss, "then\n    ");
		job_print_list(if_clause->bodies->data[i], ss);
	}
	ss_push_string(ss, "\nfi ");
}

void job_print_command(CommandP *command, StringStream *ss) {
	if (command->redir_list)
		job_print_redirlist(command->redir_list, ss);
	switch (command->type) {
		case Simple_Command: {
			job_print_stringlist(command->simple_command->assign_list, ss);
			job_print_stringlist(command->simple_command->word_list, ss);
			job_print_redirlist(command->simple_command->redir_list, ss);
			break;
		}
		case Brace_Group : { 
			ss_push_string(ss, "{ ");
			job_print_list(command->brace_group, ss); 
			ss_push_string(ss, "} ");
			break;
		}
		case Subshell: { 
			ss_push_string(ss, "( ");
			job_print_list(command->subshell, ss); 
			ss_push_string(ss, ") ");
			break;
		}
		case If_Clause: { 
			job_print_if(command->if_clause, ss);
			break;
		}
		case For_Clause: {
			ss_push_string(ss, "for ");
			ss_push_string(ss, command->for_clause->iterator);
			ss_push_string(ss, " in ");
			job_print_stringlist(command->for_clause->word_list, ss);
			ss_push_string(ss, "do ");
			job_print_list(command->for_clause->body, ss);
			ss_push_string(ss, "done ");
			break;
		}
		case Case_Clause: {
			//TODO: case 
			job_print_stringlistvect(command->case_clause->patterns, ss);
			job_print_listvect(command->case_clause->bodies, ss);
			break;
		}
		case Until_Clause:
		case While_Clause: {
			if (command->type == Until_Clause)
				ss_push_string(ss, "until ");
			else
				ss_push_string(ss, "while ");
			job_print_list(command->while_clause->body, ss);
			ss_push_string(ss, "do ");
			job_print_list(command->while_clause->condition, ss);
			ss_push_string(ss, "done ");
			break;
		}
		case Function_Definition: {
			ss_push_string(ss, command->function_definition->function_name);
			ss_push_string(ss, " ()\n");
			job_print_command(command->function_definition->function_body, ss);
			break;
		}
		default: {}
	}
}

void job_print_pipeline(PipeLineP *pipeline, StringStream *ss) {
	PipeLineP *head = pipeline;
	if (head->banged)
		ss_push_string(ss, "! ");
	while (head) {
		gc(GC_MOVE, head, GC_SUBSHELL, GC_ENV);
		job_print_command(head->command, ss);
		if (head->next)
			ss_push_string(ss, "| ");
		head = head->next;
	}
}

void job_print_andor(AndOrP * const andor, StringStream * const ss) {
	AndOrP *head = andor;
	while (head) {
		job_print_pipeline(head->pipeline, ss);
		if (head->separator == AND_IF || head->separator == OR_IF)
			ss_push_string(ss, tokenTypeStr(head->separator));
		if (head->next)
			da_push(ss, ' ');
		head = head->next;
	}
}

void job_print_list(ListP * const list, StringStream * const ss) {
	ListP *head = list;
	while (head) {
		job_print_andor(head->and_or, ss);
		if (head->separator != END)
			ss_push_string(ss, tokenTypeStr(head->separator));
		if (head->next)
			da_push(ss, ' ');
		head = head->next;
	}
}

char *job_print(AndOrP * const andor, const bool newline) {
	da_create(ss, StringStream, sizeof(char), GC_ENV);
	job_print_andor(andor, ss);
	if (newline) da_push(ss, '\n');
	return ss_get_owned_slice(ss);
}
