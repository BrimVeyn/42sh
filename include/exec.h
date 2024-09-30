/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:18:00 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/30 13:03:05 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

#include "ast.h"
#include "parser.h"
#include <stdbool.h>

typedef enum {
	E_ISDIR = 1,
	E_NOPERM,
} exec_ernno;

//----Secure mes couilles oui !
int secure_fork(void);
bool secure_dup2(int from, int to);
void secure_pipe2(int pipefd[2], int flags);
void secure_execve(const char *pathname, char **const argv, char **const envp);
int exec_node(Node *node, Vars *env);
char *find_bin_location(char *bin, StringList *env);
void close_saved_fds(int *saved_fds);
void close_std_fds(void);
void close_all_fds(void);



void builtin_set(const SimpleCommand *, Vars *);
void builtin_env(const SimpleCommand *, Vars *);
void builtin_echo(const SimpleCommand *command, Vars *shell_vars);
void builtin_exit(const SimpleCommand *command, Vars *shell_vars);

#endif
