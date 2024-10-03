/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:18:00 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/03 17:12:01 by bvan-pae         ###   ########.fr       */
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

int secure_fork(void);
bool secure_dup2(int from, int to);
void secure_pipe2(int pipefd[2], int flags);
void secure_execve(const char *pathname, char **const argv, char **const envp);
int exec_node(Node *node, Vars *env);
char *find_bin_location(char *bin, StringList *env);
void close_saved_fds(int *saved_fds);
void close_std_fds(void);
void close_all_fds(void);
bool is_builtin(char *bin);

#define TABLE_SIZE 1000

typedef struct Entry {
	int	 hits;
	char *command;
	char *bin;
	struct Entry *next; //to handle collisions
} Entry;

typedef enum {
	HASH_ADD_USED,
	HASH_ADD_UNUSED,
	HASH_FIND,
	HASH_REMOVE,
	HASH_CLEAR,
	HASH_PRINT,
	HASH_GET,
} hash_mode;

//------------------------Builtins-----------------------------//
void builtin_set(const SimpleCommand *command, Vars *shell_vars);
void builtin_env(const SimpleCommand *command, Vars *shell_vars);
void builtin_echo(const SimpleCommand *command, Vars *shell_vars);
void builtin_exit(const SimpleCommand *command, Vars *shell_vars);
void builtin_export(const SimpleCommand *command, Vars *shell_vars);
void builtin_type(const SimpleCommand *command, Vars *shell_vars);
void builtin_hash(const SimpleCommand *command, Vars *shell_vars);
void builtin_pwd(const SimpleCommand *command, Vars *shell_vars);
void builtin_cd(const SimpleCommand *command, Vars *shell_vars);
void builtin_unset(const SimpleCommand *command, Vars *shell_vars);
void *hash_interface(hash_mode mode, char *arg, Vars *shell_vars);
char *hash_find_bin(char *bin, Vars *shell_vars);
//-------------------------------------------------------------//
//

#endif
