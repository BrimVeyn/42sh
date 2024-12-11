/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jobs_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:20:17 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/11 11:57:17 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"
#include "jobs.h"
#include "utils.h"

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int job_stopped(AndOrP *j) {
	PipeLineP *p;

	for (p = j->pipeline; p; p = p->next)
		if (!p->completed && !p->stopped) {
			return 0;
		}
	return 1;
}

void job_killall(__attribute__((unused)) int code) {
	for (size_t i = 0; i < g_jobList->size; i++) {
		killpg(g_jobList->data[i]->pgid, SIGTERM);
	}
	g_jobList->size = 0;
	g_exitno = 128 + SIGQUIT;
}

int job_completed(AndOrP *j) {
	PipeLineP *p;

	for (p = j->pipeline; p; p = p->next)
		if (!p->completed) {
			return 0;
		}
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
				ft_dprintf(2, "[%ld]\tDone\t%s\n", el->id, job_print(el, false));
		} else if (job_stopped(el) && !el->notified) {
			ft_dprintf(2, "[%ld]\t%d\n", el->id, el->pgid);
			el->notified = true;
		}
	}

	for (int idx; (idx = get_completed_job_index()) != -1;) {
		da_erase_index(g_jobList, (size_t)idx);
	}
}
