/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jobs.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 13:33:36 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/09 16:22:36 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef JOBS_H
#define JOBS_H

#include "final_parser.h"

typedef struct {
	AndOrP **data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int	gc_level;
} JobList;

extern JobList *g_jobList;

void	put_job_background(AndOrP *job);
int		put_job_foreground(AndOrP *job, int cont);
void	andor_move(AndOrP *job);
void	update_job_status(void);
int		job_wait(AndOrP *job);
void	job_wait_background(AndOrP *job);
void	job_notification(void);
void	job_killall(void);
int		job_stopped(AndOrP *j);
int		job_completed(AndOrP *j);
int		mark_process_andor(AndOrP *job, pid_t pid, int status, bool print);
int		mark_process_list(pid_t pid, int status, bool print);

#endif // !JOBS_H
