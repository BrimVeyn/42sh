/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   duplicate_function.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 16:23:39 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/26 14:21:54 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "utils.h"
#include "libft.h"

ListP *gc_duplicate_list(ListP *list);
FunctionP *gc_duplicate_function(FunctionP *func);

StringListL *gc_duplicate_stringlist(StringListL *stringList) {
	StringListL *self = gc_unique(StringListVect, GC_SUBSHELL);
	self->data = gc(GC_CALLOC, stringList->capacity, stringList->size_of_element, GC_SUBSHELL);
	self->size = stringList->size;
	self->capacity = stringList->capacity;
	self->gc_level = stringList->gc_level;
	self->size_of_element = stringList->size_of_element;
	for (size_t i = 0; i < stringList->size; i++) {
		self->data[i] = gc(GC_ADD, ft_strdup(stringList->data[i]), GC_SUBSHELL);
	}
	return self;
}

RedirectionL *gc_duplicate_redirlist(RedirectionL *redirList) {
	RedirectionL *self = gc_unique(StringListVect, GC_SUBSHELL);
	self->data = gc(GC_CALLOC, redirList->capacity, redirList->size_of_element, GC_SUBSHELL);
	self->size = redirList->size;
	self->capacity = redirList->capacity;
	self->gc_level = redirList->gc_level;
	self->size_of_element = redirList->size_of_element;
	for (size_t i = 0; i < redirList->size; i++) {
		self->data[i] = gc_unique(RedirectionP, GC_SUBSHELL);
		self->data[i]->filename = gc(GC_ADD, ft_strdup(redirList->data[i]->filename), GC_SUBSHELL);
		if (redirList->data[i]->io_number)
			self->data[i]->io_number = gc(GC_ADD, ft_strdup(redirList->data[i]->io_number), GC_SUBSHELL);
	}
	return self;
}

ListPVect *gc_duplicate_listvect(ListPVect *vect) {
	ListPVect *self = gc_unique(StringListVect, GC_SUBSHELL);
	self->data = gc(GC_CALLOC, vect->capacity, vect->size_of_element, GC_SUBSHELL);
	self->size = vect->size;
	self->capacity = vect->capacity;
	self->gc_level = vect->gc_level;
	self->size_of_element = vect->size_of_element;
	for (size_t i = 0; i < vect->size; i++) {
		self->data[i] = gc_duplicate_list(vect->data[i]);
	}
	return self;
}

StringListVect *gc_duplicate_stringlistvect(StringListVect *vect) {
	StringListVect *self = gc_unique(StringListVect, GC_SUBSHELL);
	self->data = gc(GC_CALLOC, vect->capacity, vect->size_of_element, GC_SUBSHELL);
	self->size = vect->size;
	self->capacity = vect->capacity;
	self->gc_level = vect->gc_level;
	self->size_of_element = vect->size_of_element;
	for (size_t i = 0; i < vect->size; i++) {
		self->data[i] = gc_duplicate_stringlist(vect->data[i]);
	}
	return self;
}

CommandP *gc_duplicate_command(CommandP *command) {
	CommandP *self = gc_unique(CommandP, GC_SUBSHELL);
	self->type = command->type;
	if (command->redir_list)
		self->redir_list = gc_duplicate_redirlist(command->redir_list);
	switch (command->type) {
		case Simple_Command: {
			self->simple_command = gc_unique(SimpleCommandP, GC_SUBSHELL);
			self->simple_command->assign_list = gc_duplicate_stringlist(command->simple_command->assign_list);
			self->simple_command->word_list = gc_duplicate_stringlist(command->simple_command->word_list);
			self->simple_command->redir_list = gc_duplicate_redirlist(command->simple_command->redir_list);
			break;
		}
		case Brace_Group : { self->brace_group = gc_duplicate_list(command->brace_group); break;}
		case Subshell: { self->subshell = gc_duplicate_list(command->subshell); break;}
		case If_Clause: { 
			self->if_clause = gc_unique(IFClauseP, GC_SUBSHELL);
			self->if_clause->bodies = gc_duplicate_listvect(command->if_clause->bodies); 
			self->if_clause->conditions = gc_duplicate_listvect(command->if_clause->conditions); 
			break;
		}
		case For_Clause: {
			self->for_clause = gc_unique(ForClauseP, GC_SUBSHELL);
			self->for_clause->iterator = gc(GC_ADD, ft_strdup(command->for_clause->iterator), GC_SUBSHELL);
			self->for_clause->in = command->for_clause->in;
			if (self->for_clause->word_list)
				self->for_clause->word_list = gc_duplicate_stringlist(command->for_clause->word_list);
			self->for_clause->body = gc_duplicate_list(command->for_clause->body);
			break;
		}
		case Case_Clause: {
			self->case_clause = gc_unique(CaseClauseP, GC_SUBSHELL);
			self->case_clause->expression = gc(GC_ADD, ft_strdup(command->case_clause->expression), GC_SUBSHELL);
			self->case_clause->patterns = gc_duplicate_stringlistvect(command->case_clause->patterns);
			self->case_clause->bodies = gc_duplicate_listvect(command->case_clause->bodies);
			break;
		}
		case Until_Clause:
		case While_Clause: {
			self->while_clause = gc_unique(WhileClauseP, GC_SUBSHELL);
			self->while_clause->body = gc_duplicate_list(command->while_clause->body);
			self->while_clause->condition = gc_duplicate_list(command->while_clause->condition);
			break;
		}
		case Function_Definition: {
			self->function_definition = gc_duplicate_function(command->function_definition);
			break;
		}
		default: {}
	}
	return self;
}

PipeLineP *gc_duplicate_pipeline(PipeLineP *pipeline) {
	PipeLineP *self = gc_unique(PipeLineP, GC_SUBSHELL);
	PipeLineP *head = pipeline;
	PipeLineP *it = self;
	while (head) {
		it->command = gc_duplicate_command(head->command);
		it->banged = head->banged;
		if (head->next)
			it->next = gc_unique(PipeLineP, GC_SUBSHELL);
		it = it->next;
		head = head->next;
	}
	return self;
}

AndOrP *gc_duplicate_andor(AndOrP *andor) {
	AndOrP *self = gc_unique(AndOrP, GC_SUBSHELL);
	AndOrP *head = andor;
	AndOrP *it = self;
	while (head) {
		it->pipeline = gc_duplicate_pipeline(head->pipeline);
		it->separator = head->separator;
		if (head->next)
			it->next = gc_unique(AndOrP, GC_SUBSHELL);
		it = it->next;
		head = head->next;
	}
	return self;
}

ListP *gc_duplicate_list(ListP *list) {
	ListP *self = gc_unique(ListP, GC_SUBSHELL);
	ListP *head = list;
	ListP *it = self;
	while (head) {
		it->and_or = gc_duplicate_andor(head->and_or);
		it->separator = head->separator;
		if (head->next)
			it->next = gc_unique(ListP, GC_SUBSHELL);
		it = it->next;
		head = head->next;
	}
	return self;
}

FunctionP *gc_duplicate_function(FunctionP *func) {
	FunctionP *self = gc_unique(FunctionP, GC_SUBSHELL);
	self->function_name = gc(GC_ADD, ft_strdup(func->function_name), GC_SUBSHELL);
	self->function_body = gc_duplicate_command(func->function_body);
	return self;
}
