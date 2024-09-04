/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 11:41:08 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/03 16:45:53 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

#ifndef MAXISH_H
	#include "../../include/42sh.h"
#endif // !MAXISH_H

typedef enum {
	E_ISDIR = 1,
	E_NOPERM,
} exec_ernno;

int secure_fork(void);
bool secure_dup2(int from, int to);
void secure_pipe2(int pipefd[2], int flags);
void secure_execve(const char *pathname, char *const argv[], char *const envp[]);
int exec_node(Node *node, char **env);
char *find_bin_location(char *bin, char **env);

#endif
