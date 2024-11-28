/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jobs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:12:17 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/28 16:26:47 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "colors.h"
#include "final_parser.h"
#include "signals.h"
#include "utils.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <termios.h>

void job_wait (AndOrP *job) {
	int status;
	pid_t pid;
	ShellInfos *shell_infos = shell(SHELL_GET);
	da_create(list, JobListe, sizeof(AndOrP *), GC_ENV);
	list->data[list->size++] = job;

	// dprintf(2, "is it stopped : %s\n", job->first_process->stopped ? "true" : "false");	
	do {
	back_to_wait:
		pid = waitpid(-job->pgid, &status, WUNTRACED);
		// dprintf(2, C_BRIGHT_CYAN"WAIT"C_RESET": waiting for | waited | in process: "C_MAGENTA"%d | %d | %d"C_RESET"\n", job->pgid, pid, getpid());
		if (pid != -1) {
		// } else {
			// dprintf(2, C_BRIGHT_CYAN"WAIT"C_RESET": waiting for | waited | in process: "C_RED"%d | %d | %d"C_RESET"\n", job->pgid, pid,  getpid());
			//
			if (WIFSTOPPED(status) && shell_infos->script) {
				pid_t pgid = shell_infos->shell_pgid; // Foreground process group
				if (killpg(pgid, SIGTSTP) < 0) {
					perror("killpg");
					fatal("Jobs: killpg failed", 255);
				}
				if (tcsetpgrp(shell_infos->shell_terminal, job->pgid) < 0) {
					perror("tcsetpgrp(shell)");
					fatal("Jobs: tcsetpgrp(shell) failed", 255);
				}
				if (killpg(job->pgid, SIGCONT) < 0) {
					perror("killpg");
					fatal("Jobs: killpg failed", 255);
				}
				goto back_to_wait;
			}
		}
	} while (!mark_process(list, pid, status, true)
	&& !job_stopped(job)
	&& !job_completed(job));
}

#include "signal.h"

void job_wait_2 (AndOrP *job) {
	int status;
	pid_t pid;
	ShellInfos *shell_infos = shell(SHELL_GET);
	da_create(list, JobListe, sizeof(AndOrP *), GC_ENV);
	list->data[list->size++] = job;

	do {
		pid = waitpid(-1, &status, WUNTRACED);
		// dprintf(2, C_BRIGHT_CYAN"WAIT2"C_RESET": waiting for | waited | in process: "C_MAGENTA"%d | %d | %d"C_RESET"\n", job->pgid, pid, getpid());
		if (pid != -1) {

		// } else {
			// dprintf(2, C_BRIGHT_CYAN"WAIT2"C_RESET": waiting for | waited | in process: "C_RED"%d | %d | %d"C_RESET"\n", job->pgid, pid,  getpid());
		// 	perror("WAIT");
			if (WIFSTOPPED(status) && shell_infos->interactive && !shell_infos->script) {
				// dprintf(2, "pid = %d\n", getpid());
				signal_manager(SIG_EXEC);
				mark_process(list, pid, status, true);
				if (kill(getpid(), SIGTSTP) < 0) {
					perror("KILL");
					fatal("Jobs: kill failed", 255);
				}
				signal_manager(SIG_PROMPT);
				continue;
			}
			// } else if (shell_infos->interactive && shell_infos->script)
		}
            
	} while (!mark_process(list, pid, status, false)
	// && !job_stopped(job)
	&& !job_completed(job));

}

void put_job_background (AndOrP *job, bool add) {
	job->bg = true;
	if (add) {
		gc_move_andor(job);
		job->id = g_jobList->size;
		da_push(g_jobList, job);
		if (!job->subshell)
			dprintf(2, "[%zu]\t%d\n", job->id, job->pgid);
		job->notified = true;
    }
}

void put_job_foreground (AndOrP *job, int cont) {
	ShellInfos *self = shell(SHELL_GET);
	/* Put the job into the foreground.  */
	tcsetpgrp(self->shell_terminal, job->pgid);
	// dprintf(2, "JOB->PGID: %d\n", job->pgid);
	/* Send the job a continue signal, if necessary.  */
	if (cont) {
		// job->notified = false;
		tcsetattr(self->shell_terminal, TCSADRAIN, &job->tmodes);
		if (kill(-job->pgid, SIGCONT) < 0)
        {
			perror("SIGCONT");
			fatal("SIGCONT failed\n", 255);
        }
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

	if (!g_jobList || !g_jobList->size) return ;
	do {
		pid = waitpid (WAIT_ANY, &status, WUNTRACED|WNOHANG);
		// dprintf(2, "CAUGHT: %d | %d\n", pid, status);
	} while (!mark_process(g_jobList, pid, status, false));
}

int mark_process (JobListe *list, pid_t pid, int status, bool print) {
	PipeLineP *p;

	if (pid <= 0)
		return -1;

	bool flag = false;
	for (size_t i = 0; i < list->size; i++) {
		AndOrP *job = list->data[i];
		if (job->pgid == pid && !WIFSTOPPED(status)) {
			job->completed = true;
			flag = true;
			// return 0; //fixes notifications 
		}
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
					gc_move_andor(job);
					job->id = g_jobList->size;
					da_push(g_jobList, job);
					job->sig = WSTOPSIG(status);
					if (print && !job->notified)
						dprintf(STDERR_FILENO, "\n[%zu]\tStopped(%s)\t%s\n", job->id, sigStr(job->sig), job_print(job, false));
					job->notified = true;
				} else if (WIFSIGNALED (status)) {
					g_exitno = 128 + WTERMSIG(status);
					p->completed = 1;
					// dprintf (2, "%d: Terminated by signal %d.\n", (int) pid, WTERMSIG (p->status));
				}
				return 0;
			}
		}
	}
	if (flag) return 0;
	// fprintf (stderr, C_RED"FATAL: No child process %d."C_RESET"\n", pid);
	return -1;
}
