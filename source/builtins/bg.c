/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bg.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 13:04:09 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/17 13:13:40 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "parser.h"
#include "utils.h"
#include <signal.h>
#include <unistd.h>

void revive_job(Job *job) {
	job->bg = true;
	if (kill(-job->pgid, SIGCONT) < 0)
		fatal("SIGCONT failed\n", 255);
}

void builtin_bg(const SimpleCommand *command, __attribute__((unused)) Vars *shell_vars) {
	bool has_arg = command->args[1] != NULL;
	size_t job_number = 0;
	if (has_arg)
		job_number = ft_atol(command->args[1]);

	if (has_arg && job_number < job_list->size) {
		Job *job = job_list->data[job_list->size - 1 - job_number];
		revive_job(job);
	} else if (!has_arg && job_list->size) {
		Job *last_job = job_list->data[job_list->size - 1];
		revive_job(last_job);
	} else if (!has_arg) {
		ERROR_NO_SUCH_JOB("fg", "current");
	} else {
		ERROR_NO_SUCH_JOB("fg", command->args[1]);
	}
}
