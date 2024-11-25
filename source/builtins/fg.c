/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fg.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/04 17:14:34 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/25 11:44:34 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "parser.h"
#include "utils.h"
#include <unistd.h>

void builtin_fg(const SimpleCommandP *command, __attribute__((unused)) Vars *shell_vars) {
	bool has_arg = command->word_list->data[1] != NULL;
	size_t job_number = 0;
	if (has_arg)
		job_number = ft_atol(command->word_list->data[1]);

	if (has_arg && job_number < g_jobList->size) {
		AndOrP *last_job = g_jobList->data[g_jobList->size - 1 - job_number];
		last_job->bg = false;
		last_job->notified = false;
		put_job_foreground(last_job, true);
	} else if (!has_arg && g_jobList->size) {
		AndOrP *last_job = g_jobList->data[g_jobList->size - 1];
		last_job->bg = false;
		last_job->notified = false;
		put_job_foreground(last_job, true);
	} else if (!has_arg) {
		ERROR_NO_SUCH_JOB("fg", "current");
	} else {
		ERROR_NO_SUCH_JOB("fg", command->word_list->data[1]);
	}
}
