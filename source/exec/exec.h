/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 11:41:08 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/29 09:15:03 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

# pragma once
#include "../../include/minishell.h"
#include "../parser/parser.h"
#include <fcntl.h>
#include <sys/wait.h>

int secure_fork(void);
void secure_dup2(int oldfd, int newfd);
void secure_pipe2(int pipefd[2], int flags);
void secure_execve(const char *pathname, char *const argv[], char *const envp[]);
void exec_simple_command(SimpleCommand *command);

#endif
