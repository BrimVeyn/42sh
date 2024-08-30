/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 11:41:08 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/30 11:41:42 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

#ifndef MAXISH_H
	#include "../../include/42sh.h"
#endif // !MAXISH_H
#ifndef PARSER_H
	#include "../parser/parser.h"
#endif // !PARSER_H
#include <fcntl.h>
#include <sys/wait.h>


int secure_fork(void);
void secure_dup2(int oldfd, int newfd);
void secure_pipe2(int pipefd[2], int flags);
void secure_execve(const char *pathname, char *const argv[], char *const envp[]);
void exec_simple_command(SimpleCommand *command);
char *find_bin_location(char *bin);

#endif
