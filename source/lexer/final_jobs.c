/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_jobs.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:12:17 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/15 17:20:06 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "colors.h"
#include "final_parser.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <termios.h>

JobListe *jobList;

void job_wait (AndOrP *job) {
	int status;
	pid_t pid;
	da_create(list, JobListe, sizeof(AndOrP *), GC_ENV);
	list->data[list->size++] = job;

	// dprintf(2, "is it stopped : %s\n", job->first_process->stopped ? "true" : "false");
	do {
		pid = waitpid(-job->pgid, &status, WUNTRACED);
		if (pid != -1) {
			dprintf(2, C_BRIGHT_CYAN"WAIT"C_RESET": waiting for | waited | in process: "C_MAGENTA"%d | %d | %d"C_RESET"\n", job->pgid, pid, getpid());
		} else {
			dprintf(2, C_BRIGHT_CYAN"WAIT"C_RESET": waiting for | waited | in process: "C_RED"%d | %d | %d"C_RESET"\n", job->pgid, pid,  getpid());
		}
	} while (!mark_process(list, pid, status)
	&& !job_stopped(job)
	&& !job_completed(job));

}

void put_job_background (AndOrP *job) {
	job->bg = true;
	andor_move(job);
	da_push(jobList, job);
	dprintf(2, "[%zu]\t%d\n", job->id, job->pgid);
}

void put_job_foreground (AndOrP *job, int cont) {
	ShellInfos *self = shell(SHELL_GET);
	/* Put the job into the foreground.  */
	tcsetpgrp(self->shell_terminal, job->pgid);
	/* Send the job a continue signal, if necessary.  */
	if (cont) {
		job->notified = false;
		tcsetattr(self->shell_terminal, TCSADRAIN, &job->tmodes);
		if (kill(-job->pgid, SIGCONT) < 0)
			fatal("SIGCONT failed\n", 255);
	}
	/* Wait for it to report.  */
	job_wait(job);
	/* Put the self->shell back in the foreground.  */
	tcsetpgrp(self->shell_terminal, self->shell_pgid);
	/* Restore the self->shell’s terminal modes.  */
	tcgetattr(self->shell_terminal, &job->tmodes);
	tcsetattr(self->shell_terminal, TCSADRAIN, &self->shell_tmodes);
}

void update_job_status(void) {
	int status;
	pid_t pid;

	if (!job_list || !job_list->size) return ;
	do {
		pid = waitpid (WAIT_ANY, &status, WUNTRACED|WNOHANG);
	} while (!mark_process_status(job_list, pid, status));
}

int mark_process (JobListe *list, pid_t pid, int status) {
	PipeLineP *p;

	if (pid <= 0)
		return -1;

	for (size_t i = 0; i < list->size; i++) {
		AndOrP *job = list->data[i];
		/* Update the record for the process.  */
		for (p = job->pipeline; p; p = p->next) {
			if (p->pid == pid) {
				p->status = status;
				if (WIFEXITED(status)) {
					g_exitno = WEXITSTATUS(status);
					p->completed = 1;
					// dprintf (2, C_LIGHT_MAGENTA"EXIT"C_RESET": "C_GOLD"%d"C_RESET" status: "C_BRIGHT_BLUE"%d"C_RESET"\n", pid, WEXITSTATUS(status));
				} else if (WIFSTOPPED (status)) {
					p->stopped = 1;
					andor_move(job);
					da_push(jobList, job);
					job->sig = WSTOPSIG(status);
					dprintf(STDERR_FILENO, "[%zu]\tStopped(%s)\t%s\n", job->id, sigStr(job->sig), "waouh");
					job->notified = true;
				} else if (WIFSIGNALED (status)) {
					p->completed = 1;
					// dprintf (2, "%d: Terminated by signal %d.\n", (int) pid, WTERMSIG (p->status));
				}
				return 0;
			}
		}
	}
	fprintf (stderr, C_RED"FATAL: No child process %d."C_RESET"\n", pid);
	return -1;
}
