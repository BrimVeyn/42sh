/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jobs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 11:19:35 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/10 11:20:30 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "utils.h"
#include "jobs.h"
#include "signal.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <termios.h>

#include "colors.h"

int job_wait(AndOrP *job) {
	int status;
	pid_t pid;

	do {
		pid = waitpid(-job->pgid, &status, WUNTRACED);
		if (pid != -1) {
			// dprintf(2, C_BRIGHT_CYAN"WAIT "C_MAGENTA"waiting for pgid: %d | waited : %d | pgid of waited: %d | in process: %d"C_RESET"\n", job->pgid, pid, getpgid(pid), getpid());
			} else {
			// dprintf(2, C_BRIGHT_CYAN"WAIT "C_RED"waiting for pgid: %d | waited : %d | pgid of waited: %d | in process: %d"C_RESET"\n", job->pgid, pid, getpgid(pid), getpid());
		}
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
	dprintf(2, "[%zu]\t%d\n", job->id, job->pgid);
	job->notified = true;
}

int put_job_foreground (AndOrP *job, int cont) {
	ShellInfos *self = shell(SHELL_GET);
	/* Put the job into the foreground.  */
	if (tcsetpgrp(self->shell_terminal, job->pgid) == -1)
		_fatal("tcsetpgrp: failed", 1);
	// dprintf(2, "shell->pgid: %d, job->pgid: %d\n", self->shell_pgid, job->pgid);
	// dprintf(2, "JOB->PGID: %d\n", job->pgid);
	/* Send the job a continue signal, if necessary.  */
	if (cont) {
		// job->notified = false;
		if (tcsetattr(self->shell_terminal, TCSADRAIN, &job->tmodes) == -1)
			_fatal("tcsetattr: failed", 1);
		if (kill(-job->pgid, SIGCONT) < 0)
			_fatal("SIGCONT failed\n", 1);
	}
	/* Wait for it to report.  */
	job_wait(job);
	/* Put the self->shell back in the foreground.  */
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
		// dprintf(2, "CAUGHT: %d | %d\n", pid, status);
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
					dprintf(STDERR_FILENO, "\n[%zu]\tStopped(%s)\t%s\n", job->id, sigStr(job->sig), job_print(job, false));
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
						dprintf(STDERR_FILENO, "\n[%zu]\tStopped(%s)\t%s\n", job->id, sigStr(job->sig), job_print(job, false));

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
