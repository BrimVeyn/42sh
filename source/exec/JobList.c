/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JobList.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 13:38:32 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/09 15:45:07 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "exec.h"


JobList *job_list_init(void) {
	JobList *self = gc(GC_ADD, ft_calloc(1, sizeof(JobList)), GC_ENV);
	self->data = (Job **) gc(GC_ADD, ft_calloc(10, sizeof(Job *)), GC_ENV);
	self->size = 0;
	self->capacity = 10;
	return self;
}

void job_list_remove(Job *el) {
	size_t idx = 0;
	for (size_t i = 0; i < job_list->size; i++) {
		if (el->pgid == job_list->data[i]->pgid) {
			idx = i;
			job_list->data[i] = NULL;
		}
	}
	for (size_t i = idx; i < job_list->size - 1; i++) {
		job_list->data[i] = job_list->data[i + 1];
	}
	job_list->data[job_list->size - 1] = NULL;
	job_list->size--;
}

void job_list_add(Job *token) {
	if (job_list->size >= job_list->capacity) {
		job_list->capacity *= 2;
		Job **tmp = job_list->data;
		job_list->data = (Job **) ft_realloc(job_list->data, job_list->size, job_list->capacity, sizeof(Job *));
		gc(GC_FREE, tmp, GC_GENERAL);
		gc(GC_ADD, job_list->data, GC_SUBSHELL);
	}
	job_list->data[job_list->size] = token;
	job_list->size += 1;
}
