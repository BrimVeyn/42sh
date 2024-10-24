/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fg.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/04 17:14:34 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/17 13:04:56 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "parser.h"
#include "utils.h"
#include <unistd.h>

void builtin_fg(const SimpleCommand *command, __attribute__((unused)) Vars *shell_vars) {
	bool has_arg = command->args[1] != NULL;
	size_t job_number = 0;
	if (has_arg)
		job_number = ft_atol(command->args[1]);

	if (has_arg && job_number < job_list->size) {
		Job *last_job = job_list->data[job_list->size - 1 - job_number];
		last_job->bg = false;
		put_job_in_foreground(last_job, true);
	} else if (!has_arg && job_list->size) {
		Job *last_job = job_list->data[job_list->size - 1];
		last_job->bg = false;
		put_job_in_foreground(last_job, true);
	} else if (!has_arg) {
		ERROR_NO_SUCH_JOB("fg", "current");
	} else {
		ERROR_NO_SUCH_JOB("fg", command->args[1]);
	}
}
