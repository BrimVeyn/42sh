/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fg.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/04 17:14:34 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/09 17:36:58 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "jobs.h"
#include "libft.h"
#include "parser.h"
#include "utils.h"

#include <stdio.h>
#include <unistd.h>

void builtin_fg(const SimpleCommandP *command, __attribute__((unused)) Vars * const shell_vars) {
	bool has_arg = command->word_list->data[1] != NULL;
	size_t job_number = 0;
	if (has_arg)
		job_number = ft_atol(command->word_list->data[1]);

	if (has_arg && job_number < g_jobList->size) {
		
		const size_t index = g_jobList->size - 1 - job_number;
		AndOrP *last_job = g_jobList->data[index];
		last_job->bg = false;
		last_job->notified = false;

		da_erase_index(g_jobList, index)

		ft_putstr_fd(job_print(last_job, true), STDOUT_FILENO);
		put_job_foreground(last_job, true);

	} else if (!has_arg && g_jobList->size) {
		const size_t index = g_jobList->size - 1;

		AndOrP *last_job = g_jobList->data[index];
		last_job->bg = false;
		last_job->notified = false;

		da_erase_index(g_jobList, index);

		ft_putstr_fd(job_print(last_job, true), STDOUT_FILENO);
		put_job_foreground(last_job, true);


	} else if (!has_arg) {
		ERROR_NO_SUCH_JOB("fg", "current");
	} else {
		ERROR_NO_SUCH_JOB("fg", command->word_list->data[1]);
	}
}
