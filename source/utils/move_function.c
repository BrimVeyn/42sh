/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   move_function.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 16:23:39 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/26 14:20:22 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "utils.h"

void gc_move_list(ListP *list);
void gc_move_function(FunctionP *func);

void gc_move_stringlist(StringListL *stringList) {
	gc(GC_MOVE, stringList, GC_SUBSHELL, GC_ENV);
	gc(GC_MOVE, stringList->data, GC_SUBSHELL, GC_ENV);
	for (size_t i = 0; i < stringList->size; i++) {
		gc(GC_MOVE, stringList->data[i], GC_SUBSHELL, GC_ENV);
	}
}

void gc_move_redirlist(RedirectionL *redirList) {
	gc(GC_MOVE, redirList, GC_SUBSHELL, GC_ENV);
	gc(GC_MOVE, redirList->data, GC_SUBSHELL, GC_ENV);
	for (size_t i = 0; i < redirList->size; i++) {
		gc(GC_MOVE, redirList->data[i], GC_SUBSHELL, GC_ENV);
		gc(GC_MOVE, redirList->data[i]->filename, GC_SUBSHELL, GC_ENV);
		if (redirList->data[i]->io_number)
			gc(GC_MOVE, redirList->data[i]->io_number, GC_SUBSHELL, GC_ENV);
	}
}

void gc_move_listvect(ListPVect *vect) {
	gc(GC_MOVE, vect, GC_SUBSHELL, GC_ENV);
	gc(GC_MOVE, vect->data, GC_SUBSHELL, GC_ENV);
	for (size_t i = 0; i < vect->size; i++) {
		gc_move_list(vect->data[i]);
	}
}

void gc_move_stringlistvect(StringListVect *vect) {
	gc(GC_MOVE, vect, GC_SUBSHELL, GC_ENV);
	gc(GC_MOVE, vect->data, GC_SUBSHELL, GC_ENV);
	for (size_t i = 0; i < vect->size; i++) {
		gc_move_stringlist(vect->data[i]);
	}
}

void gc_move_command(CommandP *command) {
	gc(GC_MOVE, command, GC_SUBSHELL, GC_ENV);
	if (command->redir_list)
		gc_move_redirlist(command->redir_list);
	switch (command->type) {
		case Simple_Command: {
			gc(GC_MOVE, command->simple_command, GC_SUBSHELL, GC_ENV);
			gc_move_stringlist(command->simple_command->assign_list);
			gc_move_stringlist(command->simple_command->word_list);
			gc_move_redirlist(command->simple_command->redir_list);
			break;
		}
		case Brace_Group : { gc_move_list(command->brace_group); break;}
		case Subshell: { gc_move_list(command->subshell); break;}
		case If_Clause: { 
			gc(GC_MOVE, command->if_clause, GC_SUBSHELL, GC_ENV);
			gc_move_listvect(command->if_clause->bodies); 
			gc_move_listvect(command->if_clause->conditions); 
			break;
		}
		case For_Clause: {
			gc(GC_MOVE, command->for_clause, GC_SUBSHELL, GC_ENV);
			gc(GC_MOVE, command->for_clause->iterator, GC_SUBSHELL, GC_ENV);
			gc_move_list(command->for_clause->body);
			if (command->for_clause->word_list)
				gc_move_stringlist(command->for_clause->word_list);
			break;
		}
		case Case_Clause: {
			gc(GC_MOVE, command->case_clause, GC_SUBSHELL, GC_ENV);
			gc(GC_MOVE, command->case_clause->expression, GC_SUBSHELL, GC_ENV);
			gc_move_stringlistvect(command->case_clause->patterns);
			gc_move_listvect(command->case_clause->bodies);
			break;
		}
		case Until_Clause:
		case While_Clause: {
			gc(GC_MOVE, command->while_clause, GC_SUBSHELL, GC_ENV);
			gc_move_list(command->while_clause->body);
			gc_move_list(command->while_clause->condition);
			break;
		}
		case Function_Definition: {
			gc_move_function(command->function_definition);
			break;
		}
		default: {}
	}
}

void gc_move_pipeline(PipeLineP *pipeline) {
	PipeLineP *head = pipeline;
	while (head) {
		gc(GC_MOVE, head, GC_SUBSHELL, GC_ENV);
		gc_move_command(head->command);
		head = head->next;
	}
}

void gc_move_andor(AndOrP *andor) {
	AndOrP *head = andor;
	while (head) {
		gc(GC_MOVE, head, GC_SUBSHELL, GC_ENV);
		gc_move_pipeline(head->pipeline);
		head = head->next;
	}
}

void gc_move_list(ListP *list) {
	ListP *head = list;
	while (head) {
		gc(GC_MOVE, head, GC_SUBSHELL, GC_ENV);
		gc_move_andor(head->and_or);
		head = head->next;
	}
}

void gc_move_function(FunctionP *func) {
	gc(GC_MOVE, func, GC_SUBSHELL, GC_ENV);
	gc(GC_MOVE, func->function_name, GC_SUBSHELL, GC_ENV);
	gc_move_command(func->function_body);
}
