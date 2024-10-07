/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 15:09:36 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/07 17:16:23 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <signal.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "signals.h"
#include "libft.h"
#include "utils.h"
#include "exec.h"

int g_signal;

void job_table_add(JobTable *table, pid_t pid, char *bin) {
	if (table->size >= table->capacity) {
		table->capacity *= 2;
		ProcessInfos **tmp = table->data;
		table->data = ft_realloc(table->data, table->size, table->capacity, sizeof(ProcessInfos));
		gc(GC_FREE, tmp, GC_ENV);
		gc(GC_ADD, table->data, GC_ENV);
	}
	table->data[table->size] = gc(GC_CALLOC, 1, sizeof(ProcessInfos), GC_ENV);
	table->data[table->size]->id = pid;
	table->data[table->size]->bin = gc(GC_ADD, ft_strdup(bin), GC_ENV);
	table->data[table->size]->status = PROCESS_PAUSED;
	table->size += 1;
}

void job_table_update(JobTable *table, pid_t pid, char *bin, process_status status) {
	for (size_t i = 0; i < table->size; i++) {
		if (table->data[i]->id == pid) {
			table->data[i]->status = status;
			if (status == PROCESS_RESUMED) {
				kill(table->data[i]->id, SIGCONT);
			}
			return ;
		}
	}
	job_table_add(table, pid, bin);
}

void job_table_delete(JobTable *table, pid_t pid) {
	for (size_t i = 0; i < table->size; i++) {
		if (table->data[i]->id == pid) {
			gc(GC_FREE, table->data[i]->bin, GC_ENV);
			gc(GC_FREE, table->data[i], GC_ENV);
			table->data[i] = table->data[table->size - 1];
			table->data[table->size - 1] = NULL;
			table->size--;
			return ;
		}
	}
}

void job_table_print(JobTable *table) {
	char buffer[MAX_WORD_LEN] = {0};
	for (size_t i = 0; i < table->size; i++) {
		ft_sprintf(buffer, "[%ld]\t%d\t\t%s\n", i, table->data[i]->status, table->data[i]->bin);
	}
	ft_putstr_fd(buffer, STDOUT_FILENO);
}

void job(process_status status, pid_t pid, char *bin) {
	static JobTable job_table = {NULL, 0, 0};
	if (!job_table.data) {
		job_table.data = gc(GC_CALLOC, 1, sizeof(JobTable), GC_ENV);
	}
	switch (status) {
		case PROCESS_PAUSED:
		case PROCESS_RESUMED:
			job_table_update(&job_table, pid, bin, status);
			break;
		case PROCESS_EXITED:
			job_table_delete(&job_table, pid);
			break;
		case TABLE_PRINT:
			job_table_print(&job_table);
			break;
	}
}

void signal_sigint_prompt(int code) {
	rl_done = 1;
	g_signal = code;
}

void signal_sigint_exec(int code) {
	printf("\n");
	g_signal = code;
}

void signal_prompt_mode(void) {
	signal(SIGINT, signal_sigint_prompt);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	// signal(SIGCHLD, SIG_IGN);
}

void signal_exec_mode(void) {
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTSTP, SIG_DFL);
	signal(SIGTTIN, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);
	signal(SIGCHLD, SIG_DFL);
}

void signal_manager(type_of_signals mode) {
	switch(mode) {
		case SIG_PROMPT:
			signal_prompt_mode();
			break;
		case SIG_EXEC:
			signal_exec_mode();
			break;
		case SIG_HERE_DOC:
			break;
		default:
			return;
	}
}

int rl_event_dummy(void) { return 1; }

