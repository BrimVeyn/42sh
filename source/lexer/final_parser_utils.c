/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_parser_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 14:45:12 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/07 16:13:52 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "utils.h"

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

void	andOrAddBack(AndOrP **lst, AndOrP *new_value, TokenType separator) {
	AndOrP	*temp;

	if (*lst == NULL)
		*lst = new_value;
	else {
		temp = *lst;
		while (temp->next)
			temp = temp->next;
    temp->separator = separator;
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

IFClauseP *ifClauseNew(ListP *condition, ListP *body) {
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

CaseClauseP *caseClauseNew(StringListL *pattern, ListP *body) {
	CaseClauseP *self = gc_unique(CaseClauseP, GC_SUBSHELL);
	da_create(pattern_list, StringListVect, sizeof(StringListL *), GC_SUBSHELL);
	da_push(pattern_list, pattern);
	da_create(bodies, ListPVect, sizeof(ListP *), GC_SUBSHELL);
	da_push(bodies, body);
	self->patterns = pattern_list;
	self->bodies = bodies;
	return self;
}

void caseClauseMerge(CaseClauseP *parent, const CaseClauseP *child) {
	while (child->patterns->size)
		da_push(parent->patterns, da_pop(child->patterns));
	while (child->bodies->size)
		da_push(parent->bodies, da_pop(child->bodies));
}

SimpleCommandP *simpleCommandNew(void) {
	SimpleCommandP *command = gc_unique(SimpleCommandP, GC_SUBSHELL);
	da_create(redir_list, RedirectionL, sizeof(RedirectionP *), GC_SUBSHELL);
	da_create(word_list, StringListL, sizeof(char *), GC_SUBSHELL);
	da_create(assignment_list, StringListL, sizeof(char *), GC_SUBSHELL);
	command->assign_list = assignment_list;
	command->word_list = word_list;
	command->redir_list = redir_list;
	return command;
}
