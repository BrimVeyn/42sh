/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_jobs.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:12:17 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/09 15:40:28 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "colors.h"
#include "utils.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <termios.h>

void wait_for_job (Job *j) {
	int status;
	pid_t pid;
	JobList *list = job_list_init();
	job_list_add(j);

	do {
		pid = waitpid(-j->pgid, &status, WUNTRACED);
		if (pid != -1) {
			// dprintf(2, C_BRIGHT_CYAN"WAIT"C_RESET": waiting for | waited | in process: "C_MAGENTA"%d | %d | %d"C_RESET"\n", j->pgid, pid, getpid());
		} else {
			// dprintf(2, C_BRIGHT_CYAN"WAIT"C_RESET": waiting for | waited | in process: "C_RED"%d | %d | %d"C_RESET"\n", j->pgid, pid,  getpid());
		}
	} while (!mark_process_status(list, pid, status)
	&& !job_is_stopped(j)
	&& !job_is_completed(j));

	gc(GC_FREE, list, GC_ENV);
	gc(GC_FREE, list->data, GC_ENV);
}

void put_job_in_background (Job *j) {
	job_move(j);
	job_list_add(j);
}

void put_job_in_foreground (Job *j, int cont) {
	ShellInfos *self = shell(SHELL_GET);
	/* Put the job into the foreground.  */
	tcsetpgrp (self->shell_terminal, j->pgid);
	/* Send the job a continue signal, if necessary.  */
	if (cont) {
		tcsetattr (self->shell_terminal, TCSADRAIN, &j->tmodes);
		if (kill (- j->pgid, SIGCONT) < 0)
			perror ("kill (SIGCONT)");
	}
	/* Wait for it to report.  */
	wait_for_job (j);
	/* Put the self->shell back in the foreground.  */
	tcsetpgrp (self->shell_terminal, self->shell_pgid);
	/* Restore the self->shell’s terminal modes.  */
	tcgetattr (self->shell_terminal, &j->tmodes);
	tcsetattr (self->shell_terminal, TCSADRAIN, &self->shell_tmodes);
}

void update_status(void) {
	int status;
	pid_t pid;

	if (!job_list) return ;
	do {
		pid = waitpid (WAIT_ANY, &status, WUNTRACED|WNOHANG);
	} while (!mark_process_status(job_list, pid, status));
}

int mark_process_status (JobList *list, pid_t pid, int status) {
	Process *p;

	if (pid <= 0)
		return -1;

	for (size_t i = 0; i < list->size; i++) {
		Job *job = list->data[i];
		/* Update the record for the process.  */
		for (p = job->first_process; p; p = p->next)
			if (p->pid == pid) {
				p->status = status;
				if (WIFEXITED(status)) {
					g_exitno = WEXITSTATUS(status);
					p->completed = 1;
					// dprintf (2, C_LIGHT_MAGENTA"EXIT"C_RESET": "C_GOLD"%d"C_RESET" status: "C_BRIGHT_BLUE"%d"C_RESET"\n", pid, WEXITSTATUS(status));
				} else if (WIFSTOPPED (status)) {
					p->stopped = 1;
					// dprintf (2, "%d: Stopped\n", pid);
				} else if (WIFSIGNALED (status)) {
					p->completed = 1;
					// dprintf (2, "%d: Terminated by signal %d.\n", (int) pid, WTERMSIG (p->status));
				}
				return 0;
			}
	}
	fprintf (stderr, C_RED"FATAL: No child process %d."C_RESET"\n", pid);
	return -1;
}
