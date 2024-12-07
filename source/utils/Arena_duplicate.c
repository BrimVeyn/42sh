/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Arena_duplicate.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 16:23:39 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/07 11:43:43 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Arena.h"
#include "final_parser.h"
#include "utils.h"

#include <stddef.h>
#include <stdint.h>
#include <stdalign.h>

StringListL *arena_dup_stringlist(ArenaAllocator *arena, StringListL *stringList) {
    StringListL *self = arena_unique(arena, StringListL);
    self->data = arena_alloc(arena, stringList->size_of_element * stringList->size, alignof(char **));
    self->size = stringList->size;
    self->capacity = stringList->capacity;
    self->gc_level = stringList->gc_level; // Not used with arena, but kept for consistency
    self->size_of_element = stringList->size_of_element;
    for (size_t i = 0; i < stringList->size; i++) {
        self->data[i] = arena_strdup(arena, stringList->data[i]);
    }
    return self;
}

RedirectionL *arena_dup_redirlist(ArenaAllocator *arena, RedirectionL *redirList) {
    RedirectionL *self = arena_unique(arena, RedirectionL);
    self->data = arena_alloc(arena, redirList->size_of_element * redirList->size, alignof(RedirectionP **));
    self->size = redirList->size;
    self->capacity = redirList->capacity;
    self->gc_level = redirList->gc_level; // Not relevant for arena
    self->size_of_element = redirList->size_of_element;
    for (size_t i = 0; i < redirList->size; i++) {
        self->data[i] = arena_alloc(arena, sizeof(RedirectionP), alignof(RedirectionP));
        self->data[i]->filename = arena_strdup(arena, redirList->data[i]->filename);
        if (redirList->data[i]->io_number) {
            self->data[i]->io_number = arena_strdup(arena, redirList->data[i]->io_number);
        }
    }
    return self;
}

ListPVect *arena_dup_listvect(ArenaAllocator *arena, ListPVect *vect) {
	ListPVect *self = gc_unique(StringListVect, GC_SUBSHELL);
	self->data = gc(GC_CALLOC, vect->capacity, vect->size_of_element, GC_SUBSHELL);
	self->size = vect->size;
	self->capacity = vect->capacity;
	self->gc_level = vect->gc_level;
	self->size_of_element = vect->size_of_element;
	for (size_t i = 0; i < vect->size; i++) {
		self->data[i] = arena_dup_list(arena, vect->data[i]);
	}
	return self;
}

StringListVect *arena_dup_stringlistvect(ArenaAllocator *arena, StringListVect *vect) {
	StringListVect *self = gc_unique(StringListVect, GC_SUBSHELL);
	self->data = gc(GC_CALLOC, vect->capacity, vect->size_of_element, GC_SUBSHELL);
	self->size = vect->size;
	self->capacity = vect->capacity;
	self->gc_level = vect->gc_level;
	self->size_of_element = vect->size_of_element;
	for (size_t i = 0; i < vect->size; i++) {
		self->data[i] = arena_dup_stringlist(arena, vect->data[i]);
	}
	return self;
}


CommandP *arena_dup_command(ArenaAllocator *arena, CommandP *command) {
    CommandP *self = arena_unique(arena, CommandP);
    self->type = command->type;
    if (command->redir_list) {
        self->redir_list = arena_dup_redirlist(arena, command->redir_list);
    }
    switch (command->type) {
        case Simple_Command: {
            self->simple_command = arena_unique(arena, SimpleCommandP);
            self->simple_command->assign_list = arena_dup_stringlist(arena, command->simple_command->assign_list);
            self->simple_command->word_list = arena_dup_stringlist(arena, command->simple_command->word_list);
            self->simple_command->redir_list = arena_dup_redirlist(arena, command->simple_command->redir_list);
            break;
        }
        case Brace_Group: {
            self->brace_group = arena_dup_list(arena, command->brace_group);
            break;
        }
        case Subshell: {
            self->subshell = arena_dup_list(arena, command->subshell);
            break;
        }
        case If_Clause: {
            self->if_clause = arena_alloc(arena, sizeof(IFClauseP), alignof(IFClauseP));
            self->if_clause->bodies = arena_dup_listvect(arena, command->if_clause->bodies);
            self->if_clause->conditions = arena_dup_listvect(arena, command->if_clause->conditions);
            break;
        }
        case For_Clause: {
            self->for_clause = arena_alloc(arena, sizeof(ForClauseP), alignof(ForClauseP));
            self->for_clause->iterator = arena_strdup(arena, command->for_clause->iterator);
            self->for_clause->in = command->for_clause->in;
            if (command->for_clause->word_list) {
                self->for_clause->word_list = arena_dup_stringlist(arena, command->for_clause->word_list);
            }
            self->for_clause->body = arena_dup_list(arena, command->for_clause->body);
            break;
        }
        case Case_Clause: {
            self->case_clause = arena_alloc(arena, sizeof(CaseClauseP), alignof(CaseClauseP));
            self->case_clause->expression = arena_strdup(arena, command->case_clause->expression);
            self->case_clause->patterns = arena_dup_stringlistvect(arena, command->case_clause->patterns);
            self->case_clause->bodies = arena_dup_listvect(arena, command->case_clause->bodies);
            break;
        }
        case Until_Clause:
        case While_Clause: {
            self->while_clause = arena_alloc(arena, sizeof(WhileClauseP), alignof(WhileClauseP));
            self->while_clause->body = arena_dup_list(arena, command->while_clause->body);
            self->while_clause->condition = arena_dup_list(arena, command->while_clause->condition);
            break;
        }
        case Function_Definition: {
            self->function_definition = arena_dup_function(arena, command->function_definition);
            break;
        }
        default: {}
    }
    return self;
}


PipeLineP *arena_dup_pipeline(ArenaAllocator *arena, PipeLineP *pipeline) {
	PipeLineP *self = arena_unique(arena, PipeLineP);
	PipeLineP *head = pipeline;
	PipeLineP *it = self;
	while (head) {
		it->command = arena_dup_command(arena, head->command);
		it->banged = head->banged;
		if (head->next)
			it->next = arena_unique(arena, PipeLineP *);
		it = it->next;
		head = head->next;
	}
	return self;
}

AndOrP *arena_dup_andor(ArenaAllocator *arena, AndOrP *andor) {
	AndOrP *self = arena_unique(arena, AndOrP);
	AndOrP *head = andor;
	AndOrP *it = self;
	while (head) {
		it->pipeline = arena_dup_pipeline(arena, head->pipeline);
		it->separator = head->separator;
		if (head->next)
			it->next = arena_unique(arena, AndOrP);
		it = it->next;
		head = head->next;
	}
	return self;
}

ListP *arena_dup_list(ArenaAllocator *arena, ListP *list) {
	ListP *self = arena_unique(arena, ListP);
	ListP *head = list;
	ListP *it = self;
	while (head) {
		it->and_or = arena_dup_andor(arena, head->and_or);
		it->separator = head->separator;
		if (head->next)
			it->next = arena_unique(arena, ListP);
		it = it->next;
		head = head->next;
	}
	return self;
}

FunctionP *arena_dup_function(ArenaAllocator *arena, FunctionP *func) {
	FunctionP *self = arena_unique(arena, FunctionP);
	self->function_name = arena_strdup(arena, func->function_name);
	self->function_body = arena_dup_command(arena, func->function_body);
	return self;
}
