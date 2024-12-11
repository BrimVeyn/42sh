/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_secure_functions.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 16:52:42 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/11 10:51:34 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE
#include <unistd.h>
#include "exec.h"
#include <fcntl.h>
#include <stdio.h>

static int is_valid_fd(int fd) {
	return fcntl(fd, F_GETFL) != -1;
}        

static bool error_bad_file_descriptor(const int fd) {
	ft_dprintf(STDERR_FILENO, "42sh: %d: Bad file descriptor\n", fd);
	return false;
}

bool secure_dup2(const int from, const int to) {
	if (!is_valid_fd(from)) return error_bad_file_descriptor(from);
	dup2(from, to);
	return true;
}
