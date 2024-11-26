/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jobs_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:20:17 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/26 10:53:18 by bvan-pae         ###   ########.fr       */
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

void remove_job(AndOrP *job) {
	size_t index = 0;
	for (;index < g_jobList->size; index++) {
		if (job == g_jobList->data[index]) {
			break;
		}
	}
	da_erase_index(g_jobList, index);
}

static int get_completed_job_index() {
	for (size_t i = 0; i < g_jobList->size; i++) {
		AndOrP *el = g_jobList->data[i];
		if (el->completed) return i;
	}
	return -1;
}

void job_notification(void) {
	update_job_status();

	for (size_t i = 0; i < g_jobList->size; i++) {
		AndOrP *el = g_jobList->data[i];
		if (el->completed) {
			if (el->bg && !el->subshell)
				dprintf(2, "[%zu]\tDone\t%s\n", el->id, job_print(el, false));
		} else if (job_stopped(el) && !el->notified) {
			dprintf(2, "[%zu]\t%d\n", el->id, el->pgid);
			el->notified = true;
		}
	}

	for (int idx; (idx = get_completed_job_index()) != -1;) {
		da_erase_index(g_jobList, (size_t)idx);
	}
}
