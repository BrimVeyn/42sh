/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_job_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:20:17 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/10 10:22:42 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "utils.h"
#include "libft.h"

#include <stdio.h>
#include <unistd.h>

int job_is_stopped(Job *j) {
	Process *p;

	for (p = j->first_process; p; p = p->next)
		if (!p->completed && !p->stopped) {
			// printf("stopped no\n");
			return 0;
		}
	// printf("stopped yes\n");
	return 1;
}

int job_is_completed(Job *j) {
	Process *p;

	for (p = j->first_process; p; p = p->next)
		if (!p->completed) {
			// printf("completed no\n");
			return 0;
		}
	// printf("completed yes\n");
	return 1;
}

void job_move(Job *job) {
	gc(GC_MOVE, job, GC_SUBSHELL, GC_ENV);
	for (Process *p = job->first_process; p; p = p->next) {
		gc(GC_MOVE, p, GC_SUBSHELL, GC_ENV);
		gc(GC_MOVE, p->command, GC_SUBSHELL, GC_ENV);
		gc(GC_MOVE, p->command->args, GC_SUBSHELL, GC_ENV);
		for (int i = 0; p->command->args[i]; i++) {
			gc(GC_MOVE, p->command->args[i], GC_SUBSHELL, GC_ENV);
		}
	}
}

char *get_pipeline(Job *j) {
	char buffer[MAX_WORD_LEN] = {0};
	for (Process *p = j->first_process; p; p = p->next) {
		for (int i = 0; p->command->args[i]; i++) {
			ft_sprintf(buffer, "%s ", p->command->args[i]);
		}
		if (p->next)
			ft_sprintf(buffer, "| ");
	}
	return ft_strdup(buffer);
}

void do_job_notification(void) {
	update_status();

	for (size_t i = 0; i < job_list->size; i++) {
		Job *el = job_list->data[i];
		if (job_is_completed(el)) {
			if (el->bg)
				dprintf(2, "[%zu]\tDone\t%s\n", el->id, get_pipeline(el));
			job_list_remove(el);
		} else if (job_is_stopped(el) && !el->notified) {
			dprintf(2, "[%zu]\t%d\n", el->id, el->pgid);
			el->notified = true;
		}
	}
}
