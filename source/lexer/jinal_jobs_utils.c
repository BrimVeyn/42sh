/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jinal_jobs_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:20:17 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/19 16:35:50 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"
#include "utils.h"
#include "libft.h"

#include <stdio.h>
#include <unistd.h>

int job_stopped(AndOrP *j) {
	PipeLineP *p;

	for (p = j->pipeline; p; p = p->next)
		if (!p->completed && !p->stopped) {
			// printf("stopped no\n");
			return 0;
		}
	// printf("stopped yes\n");
	return 1;
}

int job_completed(AndOrP *j) {
	PipeLineP *p;

	for (p = j->pipeline; p; p = p->next)
		if (!p->completed) {
			// printf("completed no\n");
			return 0;
		}
	// printf("completed yes\n");
	return 1;
}

void andor_move(AndOrP *job) {
	gc(GC_MOVE, job, GC_SUBSHELL, GC_ENV);
	for (PipeLineP *p = job->pipeline; p; p = p->next) {
		gc(GC_MOVE, p, GC_SUBSHELL, GC_ENV);
	}
}

void remove_job(AndOrP *job) {
	size_t index = 0;
	for (;index < jobList->size; index++) {
		if (job == jobList->data[index]) {
			break;
		}
	}
	da_erase_index(jobList, index);
}

void job_notification(void) {
	update_job_status();

	for (size_t i = 0; i < jobList->size; i++) {
		AndOrP *el = jobList->data[i];
		if (el->completed) {
			if (el->bg)
				dprintf(2, "[%zu]\tDone\t%s\n", el->id, "yeahhhh");
			remove_job(el);
		} else if (job_stopped(el) && !el->notified) {
			dprintf(2, "[%zu]\t%d\n", el->id, el->pgid);
			el->notified = true;
		}
	}
}
