/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fg.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/04 17:14:34 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/10 10:47:41 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

void builtin_fg(__attribute__((unused)) const SimpleCommand *command, __attribute__((unused)) Vars *shell_vars) {
	if (!job_list->size) 
		return ;
	Job *last_job = job_list->data[job_list->size - 1];
	last_job->bg = false;
	put_job_in_foreground(last_job, true);
}
