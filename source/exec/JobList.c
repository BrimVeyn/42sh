/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JobList.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 13:38:32 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/13 11:54:26 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "exec.h"
#include "utils.h"

#include <signal.h>

void job_killall(void) {
	for (size_t i = 0; i < job_list->size; i++) {
		kill(job_list->data[i]->pgid, SIGTERM);
	}
	job_list->size = 0;
}


JobList *job_list_init(void) {
	JobList *self = gc(GC_CALLOC, 1, sizeof(JobList), GC_ENV);
	self->data = (Job **) gc(GC_CALLOC, 10, sizeof(Job *), GC_ENV);
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

bool job_search(Job *job) {
	for (size_t i = 0; i < job_list->size; i++) {
		if (job->pgid == job_list->data[i]->pgid) {
			return true;
		}
	}
	return false;
}

void job_list_add(Job *token) {
	if (job_search(token))
		return ;
	if (job_list->size >= job_list->capacity) {
		job_list->capacity *= 2;
		Job **tmp = job_list->data;
		job_list->data = (Job **) ft_realloc(job_list->data, job_list->size, job_list->capacity, sizeof(Job *));
		gc(GC_FREE, tmp, GC_ENV);
		gc(GC_ADD, job_list->data, GC_ENV);
	}
	job_list->data[job_list->size] = token;
	job_list->data[job_list->size]->id = job_list->size + 1;
	job_list->size += 1;
}
