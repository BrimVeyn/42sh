/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bg.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 11:15:31 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/10 14:22:53 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "parser.h"
#include "jobs.h"
#include "utils.h"

#include <signal.h>
#include <unistd.h>

void revive_job(AndOrP *job) {
	job->bg = true;
	if (kill(-job->pgid, SIGCONT) < 0)
		_fatal("SIGCONT failed\n", 1);
}

void builtin_bg(const SimpleCommandP *command, __attribute__((unused)) Vars * const shell_vars) {
	bool has_arg = command->word_list->data[1] != NULL;
	size_t job_number = 0;
	if (has_arg)
		job_number = ft_atol(command->word_list->data[1]);

	if (has_arg && job_number < g_jobList->size) {
		AndOrP *job = g_jobList->data[g_jobList->size - 1 - job_number];
		revive_job(job);
	} else if (!has_arg && g_jobList->size) {
		AndOrP *last_job = g_jobList->data[g_jobList->size - 1];
		revive_job(last_job);
	} else if (!has_arg) {
		ERROR_NO_SUCH_JOB("bg", "current");
	} else {
		ERROR_NO_SUCH_JOB("bg", command->word_list->data[1]);
	}
}
