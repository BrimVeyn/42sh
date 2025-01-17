/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jobs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 11:19:35 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/29 23:39:14 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"
#include "utils.h"
#include "jobs.h"
#include "signal.h"
#include "dynamic_arrays.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <termios.h>

int job_wait(AndOrP *job) {
	int status;
	pid_t pid;

	do {
		pid = waitpid(-job->pgid, &status, WUNTRACED);
		if (pid != -1) { } else { }

	} while (!mark_process_andor(job, pid, status, true)
	&& !job_stopped(job)
	&& !job_completed(job));

	return 0;
}

void put_job_background(AndOrP *job) {
	job->bg = true;
	gc_move_andor(job);
	job->id = g_jobList->size;
	da_push(g_jobList, job);
	ft_dprintf(2, "[%ld]\t%d\n", job->id, job->pgid);
	job->notified = true;
}

int put_job_foreground (AndOrP *job, int cont) {
	ShellInfos *self = shell(SHELL_GET);
	/* Put the job into the foreground.  */
	
	_debug("DEBUG: Setting terminal foreground process group to PGID: %d\n", job->pgid);
	if (tcsetpgrp(self->shell_terminal, job->pgid) == -1)
		_fatal("tcsetpgrp: failed", 1);
	/* Send the job a continue signal, if necessary.  */
	if (cont) {
		// job->notified = false;
		if (tcsetattr(self->shell_terminal, TCSADRAIN, &job->tmodes) == -1)
			_fatal("tcsetattr: failed", 1);
		if (kill(-job->pgid, SIGCONT) < 0)
			_fatal("SIGCONT failed\n", 1);
	}
	/* Wait for it to report.  */
	/*dprintf(2, "wait_status in : %d\n", getpid());*/
	job_wait(job);
	/*dprintf(2, "AAwait_status in : %d\n", getpid());*/
	/* Put the self->shell back in the foreground.  */
	_debug("DEBUG: Restoring terminal control to shell PGID: %d\n", self->shell_pgid);
	if (tcsetpgrp(self->shell_terminal, self->shell_pgid) == -1)
		_fatal("tcsetpgrp: failed", 1);
	/* Restore the self->shell’s terminal modes.  */
	if (tcgetattr(self->shell_terminal, &job->tmodes) == -1)
		_fatal("tcgetattr: failed", 1);
	if (tcsetattr(self->shell_terminal, TCSADRAIN, &self->shell_tmodes) == -1)
		_fatal("tcsetattr: failed", 1);
	return 0;
}

void update_job_status(void) {
	int status;
	pid_t pid;

	if (!g_jobList || !g_jobList->size) return ;
	do {
		pid = waitpid (WAIT_ANY, &status, WUNTRACED|WNOHANG);
	} while (!mark_process_list(pid, status, false));
}

int mark_process_andor(AndOrP *job, pid_t pid, int status, bool print) {
	PipeLineP *p;

	if (pid <= 0)
		return -1;

	bool flag = false;
	if (job->pgid == pid && !WIFSTOPPED(status)) {
		job->completed = true;
		flag = true;
	}
	/* Update the record for the process.  */
	for (p = job->pipeline; p; p = p->next) {
		if (p->pid == pid) {
			p->status = status;
			if (WIFEXITED(status)) {
				g_exitno = WEXITSTATUS(status);
				p->completed = 1;
			} else if (WIFSTOPPED (status)) {

				p->stopped = 1;
				gc_move_andor(job);
				job->id = g_jobList->size;
				da_push(g_jobList, job);

				job->sig = WSTOPSIG(status);
				g_exitno = 128 + SIGTSTP;

				if (print && !job->notified)
					ft_dprintf(STDERR_FILENO, "\n[%ld]\tStopped(%s)\t%s\n", job->id, sigStr(job->sig), job_print(job, false));
				job->notified = true;

			} else if (WIFSIGNALED (status)) {
				job->sig = WTERMSIG(status);
				g_exitno = 128 + WTERMSIG(status);
				p->completed = 1;
			}
			return 0;
		}
	}
	if (flag) return 0;
	// fprintf (stderr, C_RED"FATAL: No child process %d."C_RESET"\n", pid);
	return -1;
}

int mark_process_list(pid_t pid, int status, bool print) {
	PipeLineP *p;

	if (pid <= 0)
		return -1;

	bool flag = false;
	for (size_t i = 0; i < g_jobList->size; i++) {
		AndOrP *job = g_jobList->data[i];
		if (job->pgid == pid && !WIFSTOPPED(status)) {
			job->completed = true;
			flag = true;
		}
		/* Update the record for the process.  */
		for (p = job->pipeline; p; p = p->next) {
			if (p->pid == pid) {
				p->status = status;
				if (WIFEXITED(status)) {
					g_exitno = WEXITSTATUS(status);
					p->completed = 1;
				} else if (WIFSTOPPED (status)) {
					p->stopped = 1;
					gc_move_andor(job);

					job->id = g_jobList->size;
					da_push(g_jobList, job);

					job->sig = WSTOPSIG(status);
					g_exitno = 128 + SIGTSTP;

					if (print && !job->notified)
						ft_dprintf(STDERR_FILENO, "\n[%ld]\tStopped(%s)\t%s\n", job->id, sigStr(job->sig), job_print(job, false));

					job->notified = true;
				} else if (WIFSIGNALED (status)) {

					job->sig = WTERMSIG(status);
					g_exitno = 128 + WTERMSIG(status);

					p->completed = 1;
				}
				return 0;
			}
		}
	}
	if (flag) return 0;
	// fprintf (stderr, C_RED"FATAL: No child process %d."C_RESET"\n", pid);
	return -1;
}
